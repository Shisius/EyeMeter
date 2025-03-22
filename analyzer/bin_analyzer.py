import os
from tqdm import tqdm
from get_pupil import PupilDetect
import numpy as np
import torch
from torch.nn import functional as F
from torch.utils.data import DataLoader
from refraction_utils import estimate_coeffs
from dataset import CustomTestVectorDataset
from net import RefractionNet
import platform
# import pickle
from matplotlib import pyplot as plt
from collections import OrderedDict
import matplotlib.patches as patches
# from src.neural_refraction.train import eval_list
# from scipy.optimize import curve_fit

class ErrorsEyeMeter:
    def __init__(self):
        self.error_priority_dct = OrderedDict([
            (0, {'short': 'Фоновая ИК засветка', 'desc': 'Затемните помещение'}),
            (1, {'short': 'Зрачки не обнаружены', 'desc': 'Направьте прибор на пациента'}),
            (2, {'short': 'Изображение вне фокуса', 'desc': 'Отрегулируйте дальность'}),
            (3, {'short': 'Нет фиксации взгляда', 'desc': 'Необходимо смотреть в камеру'}),
            (4, {'short': 'Рефлекс не яркий', 'desc': 'Дефект глаз или нет фокуса'}),
            (5, {'short': 'Слишком маленький зрачок', 'desc': 'Затемните помещение'}),
            (6, {'short': 'Слишком большой зрачок', 'desc': 'Отрегулируйте яркость'}),
            (7, {'short': 'Веко перекрывает зрачок', 'desc': 'Расширьте глаза'}),
            (8, {'short': 'Ресница в области зрачка', 'desc': 'Расширьте глаза'}),
            (9, {'short': 'Измерения не полные', 'desc': 'Возникла иная ошибка'}),
            (10, {'short': 'Монокулярное измерение', 'desc': 'Закройте второй глаз'}),])

class CollectedEyeData:
    def __init__(self):
        self.collect_data = {
            'interocular_dist': [],
            'right_eye_d': [],
            'left_eye_d': [],
            'eye_positions': [],
            'img_num': []
                             }
        self.to_upload_data = ['interocular_dist', 'right_eye_d', 'left_eye_d']

    def update(self, data_dct: dict):
        for k in self.collect_data:
            if k in data_dct:
                self.collect_data[k].append(data_dct[k])

    def upload(self):
        return {k :np.array(self.collect_data[k]).mean() for k in self.to_upload_data}

    def clear(self):
        for k in self.collect_data:
            self.collect_data[k] = []

class EyeAnalyzer:
    def __init__(self, num_imgs=40, path_to_chck='.\\weights\\only_wab.pt',
                 cfg_root='.\\weights\\my_yolo8n-seg.yaml',
                 ref_weights_path='.\\weights\\weights_common.pt',
                 load_model_path='.\\weights\\yolo_eye.pt',
                 rknn_model_path='.\\weights\\yolov8_seg.rknn',
                 verbose=False, reverse=-1, conf=0.5, backend_type='rknn'):
        self.verbose = verbose
        self.data_collector = CollectedEyeData()
        self.errors = ErrorsEyeMeter()
        if backend_type == 'rknn':
            from rknn_pupil_detection import PupilDetectRKNN
            self.pd = PupilDetectRKNN(rknn_model=self.adj_os(rknn_model_path), conf=conf, iou=0.5, imgsz=640)
        elif backend_type == 'onnx':
            from onnx_pupil_detection import PupilDetectONNX
            self.pd = PupilDetectONNX(path_to_onnx = self.adj_os('C:\\Users\\tomil\Downloads\Telegram Desktop'
                                                                 '\\my_yolo8n-seg (3).onnx'),
                                      conf=conf, iou=0.7, imgsz=640)
        elif backend_type == 'torch':
            self.pd = PupilDetect(path_to_chck=self.adj_os(path_to_chck), conf=conf,
                                  cfg_root=self.adj_os(cfg_root), load_model_path=self.adj_os(load_model_path))

        self.num_imgs = num_imgs
        self.pix2mm = 0.09267 #/1.012 #/0.95 #/0.966
        self.pd_step = 0.2  # цена деления
        input_sz = 28
        num_cls = 3
        hidden_sz = 32
        do_rate = 0.2
        num_layers = 10
        self.reverse = reverse  # -1 if predict shoul be inversed
        self.ref_net = RefractionNet(input_sz,
                                     num_cls,
                                     hidden_sz=hidden_sz,
                                     do_rate=do_rate,
                                     num_layers=num_layers)
        self.ref_net.load_state_dict(torch.load(self.adj_os(ref_weights_path)))
        self.ref_net.eval()
        self.fast = True
        # self.pseudo_run()

    def pseudo_run(self):
        print('Start pre run')
        with torch.jit.optimized_execution(False):
            with torch.inference_mode():
                result = self.pd.model.predict([np.random.randint(0, 255, (416, 640))[:, :, None].repeat(3, axis=-1)],
                                               save=False, imgsz=self.pd.imgsz, conf=self.pd.conf)
        # if not self.pd.reinit_succ:
        #     print('Try to save model')
        #     self.pd.save_model()
        #     print('Model successfuly saved')

        print('Pre run finished')

    def adj_os(self, path_file: str):
        if 'Linux' in platform.system():
            path_file = path_file.replace('\\', '/')
        return path_file

    def pack2tries(self, data, use_fast=False, img_array=None, use_bbox=True):
        tmp_4 = []
        if use_bbox:
            r_idx = 1
            l_idx = 2
        else:
            r_idx = 3
            l_idx = 4

        if use_fast:
            tmp_arr_end = range(4, 40, 4) if len(data) == 40 else range(5, 41, 4)
            tmp_arr_st = range(0, 40 - 4, 4) if len(data) == 40 else range(1, 41 - 4, 4)

            for r_st, r_end in zip(tmp_arr_st, tmp_arr_end):
                try:
                    if len(data[r_st]) == 5 and len(data[r_end]) == 5:

                        tmp_4.append({'start_frame': r_st,
                                      'end_frame': r_end,

                                      'start_mask_left': data[r_st][l_idx],
                                      'start_mask_right': data[r_st][r_idx],
                                      'start_box_left': data[r_st][l_idx],
                                      'start_box_right': data[r_st][r_idx],

                                      '0_mask_left': data[r_st][l_idx],
                                      '0_mask_right': data[r_st][r_idx],
                                      '1_mask_left': data[r_st + 1][l_idx],
                                      '1_mask_right': data[r_st + 1][r_idx],
                                      '2_mask_left': data[r_st + 2][l_idx],
                                      '2_mask_right': data[r_st + 2][r_idx],
                                      '3_mask_left': data[r_st + 3][l_idx],
                                      '3_mask_right': data[r_st + 3][r_idx],
                                      '4_mask_left': data[r_end][l_idx],
                                      '4_mask_right': data[r_end][r_idx],

                                      'end_mask_left': data[r_end][l_idx],
                                      'end_mask_right': data[r_end][r_idx],
                                      'end_box_left': data[r_end][l_idx],
                                      'end_box_right': data[r_end][r_idx],
                                      })
                except:
                    print(f'problem with frame formation, start {r_st}, end {r_end}')
        else:
            for r_st, r_end in zip(data[:-1], data[1:]):
                g = 1
                if r_end[0] - r_st[0] == 4:
                    if img_array is not None and all([img_array[i].max() > 128 for i in range(r_st[0], r_end[0], )]):
                        tmp_4.append({'start_frame': r_st[0],
                                      'end_frame': r_end[0],
                                      'start_box_left': r_st[l_idx],
                                      'start_box_right': r_st[r_idx],
                                      'start_mask_left': r_st[l_idx],
                                      'start_mask_right': r_st[r_idx],
                                      'end_box_left': r_end[l_idx],
                                      'end_box_right': r_end[r_idx],
                                      'end_mask_left': r_end[l_idx],
                                      'end_mask_right': r_end[r_idx],
                                      })
                    else:
                        tmp_4.append({'start_frame': r_st[0],
                                      'end_frame': r_end[0],
                                      'start_box_left': r_st[l_idx],
                                      'start_box_right': r_st[r_idx],
                                      'start_mask_left': r_st[l_idx],
                                      'start_mask_right': r_st[r_idx],
                                      'end_box_left': r_end[l_idx],
                                      'end_box_right': r_end[r_idx],
                                      'end_mask_left': r_end[l_idx],
                                      'end_mask_right': r_end[r_idx],
                                      })
        return tmp_4

    def get_interocular_dist(self, nn_boxes_list):
        arr_form = np.array([[nn_boxes_list[0].detach().cpu().numpy(), nn_boxes_list[1].detach().cpu().numpy()]])
        intra_oc = (((((arr_form[:, 0, 0] + arr_form[:, 0, 2]) / 2) -
                      (arr_form[:, 1, 0] + arr_form[:, 1, 2]) / 2) ** 2 +
                     (((arr_form[:, 0, 1] + arr_form[:, 0, 3]) / 2) -
                      (arr_form[:, 1, 1] + arr_form[:, 1, 3]) / 2) ** 2) ** 0.5).mean()
        return round((intra_oc * self.pix2mm) / self.pd_step) * self.pd_step

    def get_eye_diameter(self, nn_boxes_list):
        arr_form = np.array([[nn_boxes_list[0].detach().cpu().numpy(), nn_boxes_list[1].detach().cpu().numpy()]])
        rr = ((arr_form[:, 0, 2] - arr_form[:, 0, 0]).mean() + (arr_form[:, 0, 3] - arr_form[:, 0, 1]).mean()) / 2
        ll = ((arr_form[:, 1, 2] - arr_form[:, 1, 0]).mean() + (arr_form[:, 1, 3] - arr_form[:, 1, 1]).mean()) / 2
        return (round(ll * self.pix2mm / self.pd_step) * self.pd_step,
                round(rr * self.pix2mm / self.pd_step) * self.pd_step)

    def get_eye_positions(self, nn_boxes_list):
        right = nn_boxes_list[0]
        left = nn_boxes_list[1]
        left_x = int((left[2] + left[0]) / 2)
        left_y = int((left[3] + left[1]) / 2)
        right_x = int((right[2] + right[0]) / 2)
        right_y = int((right[3] + right[1]) / 2)
        right_r = int(((right[2] - right[0] + right[3] - right[1]) / 4).round())
        left_r = int(((left[2] - left[0] + left[3] - left[1]) / 4).round())

        return {'n_frame': 0,
                'left_x': left_x,
                'left_y': left_y,
                'left_r': left_r,
                'right_x': right_x,
                'right_y': right_y,
                'right_r': right_r
                }


    def calculate_refraction(self, part_collections, img_array):
        info_storage = []
        for part in part_collections:
            try:
                zer_res = estimate_coeffs(img_array, part)
                info_storage.append({'processed_eyes': zer_res,
                                     'metadata': part,
                                     'subset': 'val'})
                if self.verbose:
                    for z in zer_res:
                        plt.subplot(121)
                        plt.imshow(z['left']['flickless_pupil'])
                        plt.subplot(122)
                        plt.imshow(z['right']['flickless_pupil'])
                        plt.show()
            except Exception as e:
                print(f'An error during Zernike getting: {e}')
        val_dataset = CustomTestVectorDataset(info_storage)
        dataloader = DataLoader(val_dataset, batch_size=128, shuffle=False)
        out_lst = []

        for rotation, Zernicke_coef, eye, pupil_rad, flick_rad in tqdm(dataloader, total=len(dataloader)):
            with torch.no_grad():
                out_lst.append([self.ref_net(Zernicke_coef, eye, rotation,
                                             pupil_rad, flick_rad).detach().cpu().numpy(),
                                rotation, eye])
        return info_storage, out_lst

    def process_image(self, img: np.ndarray) -> dict:
        with torch.jit.optimized_execution(False):
            with torch.inference_mode():
                result = self.pd.model.predict([img[:, :, None].repeat(3, axis=-1)],
                                               save=False, imgsz=self.pd.imgsz, conf=self.pd.conf)
                # if self.verbose:
                #     fig, ax = plt.subplots()
                #     ax.imshow(result[-1].orig_img[:, :,])
                #     # ax.imshow(out[-1].orig_img)
                #     if True:
                #         for b in result[-1].boxes:
                #             x, y, width, height = (int(b.xyxy[0, 0].item()), int(b.xyxy[0, 1].item()),
                #                                    int(b.xyxy[0, 2].item() - b.xyxy[0, 0].item()),
                #                                    int(b.xyxy[0, 3].item() - b.xyxy[0, 1].item()))
                #             patch = patches.Rectangle((x, y), width, height, facecolor='none', edgecolor='red', linewidth=2)
                #             ax.add_patch(patch)
                #     colors = ['red', 'green', 'blue', ]
                #     if True:
                #         for b in result[-1].masks:
                #
                #             poly = patches.Polygon(list(zip(b.xy[0][:, 0], b.xy[0][:, 1])), facecolor='none',
                #                            edgecolor=colors[2])
                #             ax.add_patch(poly)
                #     plt.show()
        if result[0].boxes.xyxy.size(0) == 2:
            boxes = result[0].boxes.xyxy
            masks = result[0].masks.xy
            if (boxes[0][0] + boxes[0][2]) / 2 < (boxes[1][0] + boxes[1][2]) / 2:
                tmp = [boxes[0], boxes[1], masks[0], masks[1]]
            else:
                tmp = [boxes[1], boxes[0], masks[1], masks[0]]
            result_dict = {'result': tmp, 'interocular_dist': self.get_interocular_dist(tmp)}
            l, r = self.get_eye_diameter(tmp)
            result_dict['right_eye_d'] = r
            result_dict['left_eye_d'] = l
            result_dict['eye_positions'] = self.get_eye_positions(tmp)
            return result_dict
        return {'result': self.errors.error_priority_dct[1]['short']}

    def process_array(self, img_array):
        result_dict = {'error_msg': 'none'}
        assert len(img_array) == self.num_imgs, f'NDArray should have {self.num_imgs} elements'
        img_array = img_array[1:, :, :] if len(img_array) == 41 else img_array
        tmp = []
        if self.fast:
            div = 1
        else:
            div = 4
        detection_result = {'result': 'Empty array!'}
        for img_num in range(0, self.num_imgs, div):
            detection_result = self.process_image(img_array[img_num])
            if not isinstance(detection_result['result'], str):
                tmp.append([img_num] + detection_result['result'])
                detection_result['img_num'] = img_num
                self.data_collector.update(detection_result)
        if len(tmp) == 0:
            return {'error_msg', detection_result['result']}

        try:
            part_collections = self.pack2tries(tmp, use_fast=self.fast, img_array=img_array)
            info_storage, out_lst = self.calculate_refraction(part_collections, img_array)
            try:
                left_pupil = info_storage[-1]['processed_eyes'][-1]['left']['flickless_pupil']
                left_pupil = F.interpolate(torch.tensor(left_pupil[None, None, :, :]).float(),
                                           size=(256, 256), mode='bilinear')[0][0].numpy().astype(np.uint8)
                right_pupil = info_storage[-1]['processed_eyes'][-1]['right']['flickless_pupil']
                right_pupil = F.interpolate(torch.tensor(right_pupil[None, None, :, :]).float(),
                                            size=(256, 256), mode='bilinear')[0][0].numpy().astype(np.uint8)
                result_dict['left_pupil'] = left_pupil
                result_dict['right_pupil'] = right_pupil
            except Exception as e:
                print(f'An error during pupil getting: {e}')

            try:
                left_skew = [d1['left']['flick_pos_rel'] for d in info_storage for d1 in d['processed_eyes']]
                right_skew = [d1['right']['flick_pos_rel'] for d in info_storage for d1 in d['processed_eyes']]
                result_dict['left_skew'] = left_skew
                result_dict['right_skew'] = right_skew
            except Exception as e:
                print(f'An error during skew getting: {e}')

            left = out_lst[0][0][out_lst[0][2] == 0].mean(0)
            right = out_lst[0][0][out_lst[0][2] == 1].mean(0)
            result_dict['sph_left'] = self.reverse * round(left[0] / 0.25) * 0.25
            result_dict['cyl_left'] = self.reverse * round(left[1] / 0.25) * 0.25
            result_dict['angle_left'] = round(left[2] / 30) * 30
            result_dict['sph_right'] = self.reverse * round(right[0] / 0.25) * 0.25
            result_dict['cyl_right'] = self.reverse * round(right[1] / 0.25) * 0.25
            result_dict['angle_right'] = round(right[2] / 30) * 30
        except:
            pass
        result_dict.update(self.data_collector.upload())
        result_dict['eye_positions'] = self.data_collector.collect_data['eye_positions'][0]
        return result_dict


if __name__ == '__main__':
    ea_inst = EyeAnalyzer(verbose=False , backend_type='torch')
    if 'Linux' in platform.system():
        fname = '/home/eye/Pictures/620_1_2024_06_12_16_02_42.bin'
    else:
        fname = 'D:\Projects\eye_blinks\data_24\\10_11_24\in_motion\\991_2024_11_10_00_26_07.bin'
    # fname = '777_2024_06_12_20_34_55.bin'

    with open(fname, 'rb') as f:
        s = f.read()
    num_frames = len(s) // (1216 * 1936)
    a = np.ndarray([num_frames, 1216, 1936], dtype=np.uint8, buffer=s)
    out_dict = ea_inst.process_array(a)
    print(out_dict)
