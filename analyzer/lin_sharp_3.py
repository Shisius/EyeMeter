from PIL import Image
from tqdm import tqdm
import numpy as np
import pickle
from scipy.signal import find_peaks, peak_widths
import torch
from torch.nn import functional as F
from src.components.sharpness.linear_sharp import both_side_line
from matplotlib import pyplot as plt


def get_pickle_file(path):
    with open(path, 'rb') as handle:
        myvar = pickle.load(handle)
    return myvar

class PupilSharp:
    def __init__(self, line_width=0, num_collect=12, sigma_gap=1.0, num_bins=100):
        self.line_width = line_width
        self.num_collect = num_collect
        self.sigma_gap = sigma_gap
        self.num_bins = num_bins

    def sharpness_1d_mod(self, arr):
        # plt.plot(arr)
        # plt.show()
        kernels1 = np.array([[0, 0, 0, 1, -1, 0, 0, 0],
                             [0, 0, 0, 1, 0, -1, 0, 0],
                   [0, 0, 1, 0, -1, 0, 0, 0],
                            [0, 0, 1, 0, 0, -1, 0, 0],
                   [0, 1, 0, 0, 0, -1, 0, 0], [0, 0, 1, 0, 0, 0, -1, 0],
                  [0, 1, 0, 0, 0, 0, -1, 0],
                             [1, 0, 0, 0, 0, 0, -1, 0],
                   [0, 1, 0, 0, 0, 0, 0, -1],
                            [1, 0, 0, 0, 0, 0, 0, -1],])

        res1 = F.conv1d(torch.tensor(arr)[None, None, :].repeat(1, 10, 1),
                       torch.tensor(kernels1)[None, :, :].float())[0,0].numpy()




        peaks, _ = find_peaks(abs(res1))
        cl, cr = 0, len(peaks) - 1
        while cl < cr:
            if cl + 1 <= cr and abs(res1)[peaks[cl]] < abs(res1)[peaks[cl + 1]]:
                cl += 1
            elif cr - 1 >= cl and abs(res1)[peaks[cr]] < abs(res1)[peaks[cr - 1]]:
                cr -= 1
            else:
                break
        prop = peak_widths(abs(res1), peaks[[cl, cr]])
        # plt.plot(peaks, abs(res1)[peaks], "x")
        # plt.plot(abs(res1))
        # plt.show()
        return prop[0]

    def sharpness_1d(self, y_arr):

        self.diff_line(y_arr)
        x_arr = np.arange(len(y_arr))
        point_l, point_r = 0, len(y_arr) - 1
        left_stop_num, right_stop_num, slope, offset, std = both_side_line(x_arr, y_arr, point_l, point_r,
                                                                           self.num_collect, self.sigma_gap)
        # for i in range(right_stop_num - left_stop_num):
        #     if y_arr[left_stop_num + i] / y_arr[left_stop_num:right_stop_num].mean() > 1:
        #         break
        # left_slope = i
        # for i in range(right_stop_num - left_stop_num):
        #     if y_arr[right_stop_num - i] / y_arr[left_stop_num:right_stop_num].mean() > 1:
        #         break
        # right_slope = i - 1
        left_slope = 0
        right_slope = 0
        x_arr_new = x_arr[left_stop_num + left_slope: right_stop_num - right_slope]
        y_arr_new = y_arr[left_stop_num + left_slope: right_stop_num - right_slope]
        fam = np.histogram(y_arr_new, self.num_bins)
        real_lvl = fam[1][fam[0].argmax()]
        plt.plot(x_arr, y_arr)
        plt.plot(x_arr_new, y_arr_new)
        plt.plot(x_arr, real_lvl * np.ones_like(x_arr))
        plt.show()


    def get_image_slice(self, image, angle):
        image = Image.fromarray(image).rotate(angle, expand=True)
        image = np.array(image)
        # plt.imshow(image)
        # plt.show()
        if self.line_width > 0:
            line = image[image.shape[0]// 2-self.line_width: image.shape[0]// 2+self.line_width, :].mean(0)
        else:
            line = image[image.shape[0]//2, :]
            line = line[line > 0]
        return line.astype(np.float32)

    def get_sharpness(self, img, *args, **kwargs):
        # plt.imshow(img)
        # plt.show()
        """take two diagonal slices of image and measure slope"""
        ang = np.arctan2(img.shape[0], img.shape[1]) / np.pi * 180
        line_45 = self.get_image_slice(img, ang)
        line_135 = self.get_image_slice(img, 90+ang)
        sh_45 = self.sharpness_1d_mod(line_45)
        sh_135 = self.sharpness_1d_mod(line_135)
        return round((sh_45.mean() + sh_135.mean())/2, 2)



if __name__ == '__main__':
    psh = PupilSharp()
    path_to_pickle = 'D:\Projects\eye_blinks\data_24\\12_06_24\\refractions_2.pickle'
    train_data = get_pickle_file(path_to_pickle)
    psh.get_sharpness(train_data[1796]['processed_eyes'][0]['left']['init_pupil'])
    sh = []
    for idx, d in tqdm(enumerate(train_data)):
        sh.append(psh.get_sharpness(d['processed_eyes'][0]['left']['init_pupil']))
        if sh[-1] > 5.5:
            print(idx)
    plt.hist(sh, 30)
    plt.show()