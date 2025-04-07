from PIL import Image
from tqdm import tqdm
import numpy as np
import pickle
from scipy.signal import find_peaks, peak_widths
import torch
from torch.nn import functional as F
# from src.components.sharpness.linear_sharp import both_side_line
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
        plt.plot(peaks, abs(res1)[peaks], "x")
        plt.plot(abs(res1))
        plt.show()
        return prop[0]


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
        return 100 - min(max((round((sh_45.mean() + sh_135.mean())/2, 2) - 3.2) / (6.1 - 3.2), 0), 1) * 100.



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