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


# from src.neural_refraction.train import eval_list
# from scipy.optimize import curve_fit


class EyeAnalyzer:
    def __init__(self, num_imgs=40, path_to_chck='.\\weights\\only_wab.pt',
                 cfg_root='.\\weights\\my_yolo8n-seg.yaml',
                 ref_weights_path='.\\weights\\weights.pt',
                 load_model_path='.\\weights\\yolo_eye.pt', verbose=False):
        self.verbose = verbose
        self.pd = PupilDetect(path_to_chck=self.adj_os(path_to_chck), conf=0.5,
                              cfg_root=self.adj_os(cfg_root), load_model_path=self.adj_os(load_model_path))
        self.num_imgs = num_imgs
        self.pix2mm = 0.09267
        input_sz = 28
        num_cls = 3
        hidden_sz = 1024
        do_rate = 0.3
        num_layers = 4

        self.ref_net = RefractionNet(input_sz,
                                     num_cls,
                                     hidden_sz=hidden_sz,
                                     do_rate=do_rate,
                                     num_layers=num_layers)
        self.ref_net.load_state_dict(torch.load(self.adj_os(ref_weights_path)))
        self.ref_net.eval()
        self.pseudo_run()

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

    def pack2tries(self, data):
        tmp_4 = []
        for r_st, r_end in zip(data[:-1], data[1:]):
            g = 1
            if r_end[0] - r_st[0] == 4:
                tmp_4.append({'start_frame': r_st[0],
                              'end_frame': r_end[0],
                              'start_box_left': r_st[2],
                              'start_box_right': r_st[1],
                              'start_mask_left': r_st[4],
                              'start_mask_right': r_st[3],
                              'end_box_left': r_end[2],
                              'end_box_right': r_end[1],
                              'end_mask_left': r_end[4],
                              'end_mask_right': r_end[3],
                              })
        return tmp_4

    def get_interocular_dist(self, nn_boxes_list):
        arr_form = np.array([[t[1].detach().cpu().numpy(), t[2].detach().cpu().numpy()] for t in nn_boxes_list])
        intra_oc = (((((arr_form[:, 0, 0] + arr_form[:, 0, 2]) / 2) -
                      (arr_form[:, 1, 0] + arr_form[:, 1, 2]) / 2) ** 2 +
                     (((arr_form[:, 0, 1] + arr_form[:, 0, 3]) / 2) -
                      (arr_form[:, 1, 1] + arr_form[:, 1, 3]) / 2) ** 2) ** 0.5).mean()
        return round(intra_oc * self.pix2mm, 2)

    def get_eye_diameter(self, nn_boxes_list):
        arr_form = np.array([[t[1].detach().cpu().numpy(), t[2].detach().cpu().numpy()] for t in nn_boxes_list])
        rr = ((arr_form[:, 0, 2] - arr_form[:, 0, 0]).mean() + (arr_form[:, 0, 3] - arr_form[:, 0, 1]).mean()) / 2
        ll = ((arr_form[:, 1, 2] - arr_form[:, 1, 0]).mean() + (arr_form[:, 1, 3] - arr_form[:, 1, 1]).mean()) / 2
        return round(ll * self.pix2mm, 2), round(rr * self.pix2mm, 2)

    def get_eye_positions(self, nn_boxes_list):
        num = nn_boxes_list[-1][0]
        right = nn_boxes_list[-1][1]
        left = nn_boxes_list[-1][2]
        left_x = int((left[2] + left[0]) / 2)
        left_y = int((left[3] + left[1]) / 2)
        right_x = int((right[2] + right[0]) / 2)
        right_y = int((right[3] + right[1]) / 2)
        right_r = int(((right[2] - right[0] + right[3] - right[1]) / 4).round())
        left_r = int(((left[2] - left[0] + left[3] - left[1]) / 4).round())

        return {'n_frame': num,
                'left_x': left_x,
                'left_y': left_y,
                'left_r': left_r,
                'right_x': right_x,
                'right_y': right_y,
                'right_r': right_r
                }

    def process_array(self, img_array):
        result_dict = {}
        assert len(img_array) == self.num_imgs, f'NDArray should have {self.num_imgs} elements'
        img_array = img_array[1:, :, :] if len(img_array) == 41 else img_array
        tmp = []
        for img_num in range(0, self.num_imgs, 4):
            with torch.jit.optimized_execution(False):
                with torch.inference_mode():
                    result = self.pd.model.predict([img_array[img_num][:, :, None].repeat(3, axis=-1)],
                                                   save=False, imgsz=self.pd.imgsz, conf=self.pd.conf)
            if result[0].boxes.xyxy.size(0) == 2:
                res = result[0].boxes.xyxy
                masks = result[0].masks.xy
                if (res[0][0] + res[0][2]) / 2 < (res[1][0] + res[1][2]) / 2:
                    tmp.append([img_num, res[0], res[1], masks[0], masks[1]])  # TODO check this
                else:
                    tmp.append([img_num, res[1], res[0], masks[1], masks[0]])
        try:
            part_collections = self.pack2tries(tmp)
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
            val_dataset = CustomTestVectorDataset(info_storage)
            dataloader = DataLoader(val_dataset, batch_size=128, shuffle=False)
            out_lst = []

            for rotation, Zernicke_coef, eye, pupil_rad, flick_rad in tqdm(dataloader, total=len(dataloader)):
                with torch.no_grad():
                    out_lst.append([self.ref_net(Zernicke_coef, eye, rotation,
                                                 pupil_rad, flick_rad).detach().cpu().numpy(),
                                    rotation, eye])
            left = out_lst[0][0][out_lst[0][2] == 0].mean(0)
            right = out_lst[0][0][out_lst[0][2] == 1].mean(0)
            result_dict['sph_left'] = round(left[0] / 0.25) * 0.25
            result_dict['cyl_left'] = round(left[1] / 0.25) * 0.25
            result_dict['angle_left'] = round(left[2] / 30) * 30
            result_dict['sph_right'] = round(right[0] / 0.25) * 0.25
            result_dict['cyl_right'] = round(right[1] / 0.25) * 0.25
            result_dict['angle_right'] = round(right[2] / 30) * 30
        except:
            pass
        result_dict['interocular_dist'] = self.get_interocular_dist(tmp)
        l, r = self.get_eye_diameter(tmp)
        result_dict['right_eye_d'] = r
        result_dict['left_eye_d'] = l
        result_dict['eye_positions'] = self.get_eye_positions(tmp)
        return result_dict


if __name__ == '__main__':
    ea_inst = EyeAnalyzer(verbose=False)
    fname = 'D:\Projects\eye_blinks\data_24\\12_08_24\\_2024_08_12_18_50_05.bin'
    # fname = '777_2024_06_12_20_34_55.bin'

    with open(fname, 'rb') as f:
        s = f.read()
    num_frames = len(s) // (1216 * 1936)
    a = np.ndarray([num_frames, 1216, 1936], dtype=np.uint8, buffer=s)
    out_dict = ea_inst.process_array(a)
    print(out_dict)
