from ultralytics import YOLO
import cv2
import os
from matplotlib import pyplot as plt
import torch
import numpy as np
from collections import OrderedDict
from torch.nn import functional as F
from zernike import RZern
from PIL import Image
#from .utils import contour_finder, fit_circle, circles_find
# Load a model
# model = YOLO('yolov8n-seg.pt')  # load an official model


def crop_center(img) -> Image:
    """
    Функция для обрезки изображения по центру.
    """
    pil_img = Image.fromarray(img)
    img_width, img_height = pil_img.size
    crop_sz = min(img_width, img_height)
    croped_img = pil_img.crop(((img_width - crop_sz) // 2,
                         (img_height - crop_sz) // 2,
                         (img_width + crop_sz) // 2,
                         (img_height + crop_sz) // 2))
    return np.asarray(croped_img)


class PupilDetect:
    def __init__(self, path_to_chck='D:\\Projects\\eye_blinks\\medicoptic\\src\\yolo_v8\\'
                                    'model_train\\runs\segment\\train16\weights\\last.pt',
                        cfg_root='.\\my_yolo8n-seg.yaml',
                        load_model_path=',',
                        conf=0.5,
                        imgsz=640):
        # if os.path.isfile(load_model_path):
        #     self.model = torch.load(load_model_path)
        #     print('Model was successfully reintialized')
        #     self.reinit_succ = True
        # else:
        self.model = YOLO(cfg_root)  # build a new model from YAML
        wab = torch.load(path_to_chck)
        wab = OrderedDict([(k.replace('model.', 'model.model.'), v) for k, v in wab.items()])
        self.model.load_state_dict(wab)
        # self.model.export(format="onnx")
            # self.reinit_succ = False
        # self.model.model = self.model.model.eval()
        # self.model.fuse()
        # self.model = YOLO(path_to_chck)  # load a custom model
        self.conf = conf
        self.imgsz = imgsz
    #     self.load_model_path = load_model_path
    #
    # def save_model(self):
    #     torch.save(self.model, self.load_model_path)

    def get_flickless_pupil(self, im_path, remove_flick=True):
        gap = 7
        cut_sz = 20
        # Predict with the model
        if '.bin' in im_path:
            with open(im_path, 'rb') as f:
                s = f.read()
            img = np.ndarray([1216, 1936], dtype=np.uint8, buffer=s)
            # im = np.resize(im, (640, 640))
            results = self.model([img[:, :, None].repeat(3, axis=-1)], save=False, imgsz=self.imgsz, conf=self.conf)
        else:
            results = self.model(im_path, save=False, imgsz=self.imgsz, conf=self.conf)  # predict on an image
        if 1 in results[0].boxes.cls and 0 not in results[0].boxes.cls:
            if '.bin' not in im_path:
                img = cv2.imread(im_path, cv2.IMREAD_COLOR)
            # Convert to grayscale.
            gray = cv2.cvtColor(img, cv2.COLOR_RGB2GRAY)
            res = results[0].boxes
            cls = res.cls
            pupil_mask = cls == 1
            xyxy_p = res.xyxy[pupil_mask][0].cpu().numpy().astype(int)
            pupil = gray[xyxy_p[1]:xyxy_p[3], xyxy_p[0]:xyxy_p[2]]
            lst = circles_find(pupil, eps_cord=20, eps_rad_rel=0.1, sigma=3, low_threshold=0.5,
                               high_threshold=0.8)
            lst = [l for l in lst if 5 < l[0][1] < 50][0]

            x1, y1, x2, y2 = int(lst[0][0][0] - lst[0][1]), int(lst[0][0][1] - lst[0][1]), \
                             int(lst[0][0][0] + lst[0][1]), int(lst[0][0][1] + lst[0][1])
            x1, y1, x2, y2 = x1 - cut_sz, y1 - cut_sz, x2 + cut_sz, y2 + cut_sz
            if remove_flick:
                c1 = pupil[y1:y1 + gap, x1:x1 + gap].mean()
                c2 = pupil[y2 - gap:y2, x1:x1 + gap].mean()
                c3 = pupil[y1:y1 + gap, x2 - gap:x2].mean()
                c4 = pupil[y2 - gap:y2, x2 - gap:x2].mean()
                c1 = c2 = c3 = c4 = np.array([c1, c2, c3, c4]).mean()
                bilin = F.interpolate(torch.tensor([[c1, c3], [c2, c4]])[None, None],
                                      size=(int(y2 - y1), int(x2 - x1)), mode='bilinear')[0][0].numpy()
                pupil[y1:y2, x1:x2] = bilin + np.random.randn(y2 - y1, x2 - x1) * pupil[y1:y1 + gap, x1:x1 + gap].std()

            pupil_rad = (abs(xyxy_p[1] - xyxy_p[3]) + abs(xyxy_p[0] - xyxy_p[2])) / 4

            pupil_masks = results[0].masks.xy[torch.argmax(pupil_mask.to(torch.float)).cpu().numpy()]
            pupil_params = fit_circle(pupil_masks)
            filck_params = (x1 + x2) / 2, (y1 + y2) / 2, (abs(x2 - x1) + abs(y2 - y1)) / 4
            flick_offset = ((pupil.shape[0] / 2 - filck_params[0]) ** 2 + (
                        pupil.shape[1] / 2 - filck_params[1]) ** 2) ** 0.5
            flick_angle = np.arctan2(pupil.shape[1] / 2 - filck_params[1],
                                     pupil.shape[0] / 2 - filck_params[0]) / np.pi * 180
            eye_params = {'pupil_rad_mask': pupil_params[2], 'pupil_rad_box': pupil_rad, 'flick_offset': flick_offset,
                          'flick_rad_mask': filck_params[2], "flick_angle": flick_angle,
                          'small_eye': cv2.resize(pupil, (64, 64), interpolation=cv2.INTER_AREA)}
            return pupil, eye_params
        if len(results[0].boxes) != 2:
            return None
        res = results[0].boxes
        cls = res.cls
        pupil_mask = cls == 1
        flick_mask = cls == 0
        xyxy_p = res.xyxy[pupil_mask][0].cpu().numpy().astype(int)
        x1, y1, x2, y2 = res.xyxy[flick_mask][0].cpu().numpy().astype(int)
        if '.bin' not in im_path:
            img = cv2.imread(im_path, cv2.IMREAD_COLOR)
            # Convert to grayscale.
            gray = cv2.cvtColor(img, cv2.COLOR_RGB2GRAY)
        else:
            gray = img
        x1, y1, x2, y2 = x1 - cut_sz, y1 - cut_sz, x2 + cut_sz, y2 + cut_sz
        if remove_flick:
            c1 = gray[y1:y1 + gap, x1:x1 + gap].mean()
            c2 = gray[y2 - gap:y2, x1:x1 + gap].mean()
            c3 = gray[y1:y1 + gap, x2 - gap:x2].mean()
            c4 = gray[y2 - gap:y2, x2 - gap:x2].mean()
            c1 = c2 = c3 = c4 = np.array([c1, c2, c3, c4]).mean()
            bilin = F.interpolate(torch.tensor([[c1, c3], [c2, c4]])[None, None],
                                  size=(int(y2 - y1), int(x2 - x1)), mode='bilinear')[0][0].numpy()
            gray[y1:y2, x1:x2] = bilin + np.random.randn(y2 - y1, x2 - x1) * gray[y1:y1 + gap, x1:x1 + gap].std()

        pupil = gray[xyxy_p[1]:xyxy_p[3], xyxy_p[0]:xyxy_p[2]]
        pupil_rad = (abs(xyxy_p[1]-xyxy_p[3])+abs(xyxy_p[0]-xyxy_p[2]))/4

        pupil_masks = results[0].masks.xy[torch.argmax(pupil_mask.to(torch.float)).cpu().numpy()]
        pupil_params = fit_circle(pupil_masks)
        filck_masks = results[0].masks.xy[torch.argmax(flick_mask.to(torch.float)).cpu().numpy()]
        filck_params = fit_circle(filck_masks)
        flick_offset = ((pupil_params[0] - filck_params[0])**2 + (pupil_params[1] - filck_params[1])**2)**0.5
        flick_angle = np.arctan2(pupil_params[1] - filck_params[1], pupil_params[0] - filck_params[0])/ np.pi * 180
        eye_params = {'pupil_rad_mask': pupil_params[2], 'pupil_rad_box': pupil_rad, 'flick_offset': flick_offset,
                      'flick_rad_mask': filck_params[2], "flick_angle": flick_angle,
                      'small_eye': cv2.resize(pupil, (64, 64), interpolation=cv2.INTER_AREA)}
        # cv2.drawContours(gray, pupil_masks[:, None, :].astype(np.int32), -1, (0, 255, 0), 3)
        # #cv2.drawContours(gray, filck_masks[:, None, :].astype(np.int32) - 1, (0, 0, 255), 3)
        # cv2.imshow('Pupil', gray)
        # cv2.waitKey()
        return pupil, eye_params

    def get_zernicke_from_image(self, gray_img, radial_order=6, rad_cut=1.0, norm=False):
        cart = RZern(radial_order)
        L, K = gray_img.shape[0], gray_img.shape[1]
        ddx = np.linspace(-rad_cut, rad_cut, K)
        ddy = np.linspace(-rad_cut, rad_cut, L)
        xv, yv = np.meshgrid(ddx, ddy)
        cart.make_cart_grid(xv, yv)

        c0 = np.random.normal(size=cart.nk)
        Phi = np.copy(gray_img)
        if norm:
            Phi = Phi / Phi.mean()
        c1 = cart.fit_cart_grid(np.flipud(Phi))[0]
        return cart, Phi, c1

    def plot_zernicke(self, cart, Phi, c1, gray_img):
        plt.figure(1)
        plt.subplot(1, 2, 1)
        plt.imshow(Phi, origin='lower', extent=(-1, 1, -1, 1))
        plt.axis('off')
        plt.subplot(1, 2, 2)
        # plt.plot(range(1, cart.nk + 1), c0, marker='.')
        plt.plot(range(1, len(c1) + 1), c1, marker='.')

        plt.show()
        # plt.figure(1)
        # plt.subplot(121)
        # plt.imshow(gray_img)
        # plt.subplot(122)
        # Phi = cart.eval_grid(c1, matrix=True)
        # plt.imshow(Phi, origin='lower', extent=(-1, 1, -1, 1))
        # plt.axis('off')
        #
        # plt.show()

    def get_zernicke_from_path(self, im_path, plot=False, norm=False, rot_degree=0):
        pupil, pupil_rad = self.get_flickless_pupil(im_path, remove_flick=True)
        # pupil = crop_center(pupil)
        # pupil = cv2.blur(pupil, (10, 10))
        # pupil = ((pupil.astype(np.float64) + np.asarray(Image.fromarray(pupil).rotate(15)).astype(np.float64) +
        #          np.asarray(Image.fromarray(pupil).rotate(-15)).astype(np.float64)) / 3).astype(np.uint8)
        # pupil = np.asarray(Image.fromarray(pupil).rotate(-rot_degree))
        if pupil is None:
            return None
        cart, Phi, c1 = self.get_zernicke_from_image(pupil, radial_order=6, rad_cut=1.2, norm=norm)
        if plot:
            self.plot_zernicke(cart, Phi, c1, pupil)
        return cart, Phi, c1, pupil_rad


if __name__ == '__main__':
    im_path = 'D:\Projects\eye_blinks\data\\02_12\\582_1\left\\12.png'
    pupil_detector = PupilDetect(conf=0.99)
    pupil, pupil_rad = pupil_detector.get_flickless_pupil(im_path)
    cart, Phi, c1 = pupil_detector.get_zernicke_from_image(pupil)
    pupil_detector.plot_zernicke(cart, Phi, c1, pupil)
    # plt.imshow(pupil)
    # plt.show()
    # g = 1