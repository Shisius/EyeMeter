from PIL import Image
from tqdm import tqdm
import numpy as np
import pickle
from scipy.signal import find_peaks, peak_widths
import torch
from torch.nn import functional as F
# from src.components.sharpness.linear_sharp import both_side_line
from matplotlib import pyplot as plt
from analyzer.neural_sharp_line.net import SharpNet


def get_pickle_file(path):
    with open(path, 'rb') as handle:
        myvar = pickle.load(handle)
    return myvar

class PupilSharp:
    def __init__(self, line_width=0, num_collect=12, sigma_gap=1.0, num_bins=100,
                 device='cpu', path_to_weights='best_sharp_net.pth'):
        self.line_width = line_width
        self.num_collect = num_collect
        self.sigma_gap = sigma_gap
        self.num_bins = num_bins
        self.device = device
        self.net = None
        self.net_init(path_to_weights)

    def net_init(self, path_to_weights):
        self.net = SharpNet().to(self.device)
        wab = None
        try:
            wab = torch.load(path_to_weights)
        except:
            raise NotImplementedError('Impossible to found proper weights')
        if wab is not None:
            self.net.load_state_dict(wab, strict=True)
        self.net.eval()

    @torch.no_grad()
    def sharpness_1d_mod(self, arr):
        sh = self.net(torch.tensor(arr)[None, :].to(self.device))
        return sh[0, 0].detach().cpu().numpy()


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
        line_40 = self.get_image_slice(img, 40)
        line_50 = self.get_image_slice(img, 50)
        sh_40 = self.sharpness_1d_mod(line_40)
        sh_50 = self.sharpness_1d_mod(line_50)
        line_130 = self.get_image_slice(img, 130)
        line_140 = self.get_image_slice(img, 140)
        sh_130 = self.sharpness_1d_mod(line_130)
        sh_140 = self.sharpness_1d_mod(line_140)
        sh_mean = (2*sh_45.mean() + 2*sh_135.mean() + sh_40.mean() + sh_50.mean()+sh_130.mean() + sh_140.mean())/8
        return 100 - min(max((round(sh_mean, 2) - 1.0) / (5.0 - 1.0), 0), 1) * 100.



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
    plt.hist(sh, 100)
    plt.show()