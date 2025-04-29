import torch
from copy import deepcopy
from random import random
import numpy as np
from torch.utils.data import Dataset
from torchvision.transforms import v2
from torchvision.transforms.v2 import functional as TVF
from torch.nn import functional as F

class CustomVectorDataset(Dataset):
    def __init__(self, datalist, transform=None, target_transform=None, subset='train'):
        self.all_data = [d for d in datalist if d['subset'] == subset]
        self.eye2idx = {'left': 0, 'right': 1}
        self.angle2idx = {0: 0, 60: 1, 120: 2, 180: 3, 240: 4, 300: 5}
        self.transform = transform
        self.target_transform = target_transform

    def __len__(self):
        return len(self.all_data)

    def __getitem__(self, idx):
        data = deepcopy(self.all_data[idx])
        sph, cyl, angle = data['metadata']['sphere'], data['metadata']['cylinder'], data['metadata']['angle']
        refr = torch.tensor([sph, cyl, angle])
        if data['metadata']['start_frame'] % 8 == 0:
            rotation = 0  # 0, 60, 120 deg
        else:
            rotation = 1  # 180, 240, 300 deg
        # eye choose
        if random() > 0.5:
            eye = 'left'
        else:
            eye = 'right'

        Zernicke_coef = torch.cat([torch.tensor(d[eye]['zernicke_c'])
                                   [None, :] for d in data['processed_eyes']]).to(torch.float32)

        pupil_rad = 0
        eye = self.eye2idx[eye]
        flick_rad = 0
        return refr, rotation, Zernicke_coef, eye, pupil_rad, flick_rad


class CustomTestVectorDataset(Dataset):
    def __init__(self, datalist, transform=None, target_transform=None):
        new_dl = []
        for d in datalist:
            new_dl.append(
                {'processed_eyes': [{'left': d1['left']} for d1 in d['processed_eyes']], 'metadata': d['metadata'],
                 'subset': d['subset']})
            new_dl.append(
                {'processed_eyes': [{'right': d1['right']} for d1 in d['processed_eyes']], 'metadata': d['metadata'],
                 'subset': d['subset']})
        self.all_data = new_dl
        self.eye2idx = {'left': 0, 'right': 1}
        self.angle2idx = {0: 0, 60: 1, 120: 2, 180: 3, 240: 4, 300: 5}
        self.transform = transform
        self.target_transform = target_transform

    def __len__(self):
        return len(self.all_data)

    def __getitem__(self, idx):
        data = deepcopy(self.all_data[idx])

        if data['metadata']['start_frame'] in range(1, 41, 4):
            if (data['metadata']['start_frame'] - 1) % 8 == 0:
                rotation = 0  # 0, 60, 120 deg
            else:
                rotation = 1  # 180, 240, 300 deg
        else:
            if data['metadata']['start_frame'] % 8 == 0:
                rotation = 0  # 0, 60, 120 deg
            else:
                rotation = 1  # 180, 240, 300 deg
        eye = list(data['processed_eyes'][-1].keys())[-1]

        Zernicke_coef = torch.cat([torch.tensor(d[eye]['zernicke_c'])
                                   [None, :] for d in data['processed_eyes']]).to(torch.float32)

        pupil_rad = np.array([sz/2 for d in data['processed_eyes'] for sz in torch.tensor(d[eye]['init_pupil']).size()]).mean()
        eye = self.eye2idx[eye]
        flick_rad = 0
        return rotation, Zernicke_coef, eye, pupil_rad, flick_rad



def remove_flick(pup_1_l, gap = 6, cut_sz = 12):
    flick_pos = np.unravel_index(np.argmax(pup_1_l), np.array(pup_1_l).shape)
    x1, y1, x2, y2 = flick_pos[1], flick_pos[0], flick_pos[1], flick_pos[0]
    x1, y1, x2, y2 = x1 - cut_sz, y1 - cut_sz, x2 + cut_sz, y2 + cut_sz
    if x1 < 0 and x2 - x1 < 2:
        return None
    if y1 < 0 and y2 - y1 < 2:
        return None
    if x2 < 2:
        return None
    if y2 < 2:
        return None

    if y2 - gap < 1:
        return None
    if x2 - gap < 1:
        return None
    if x1 + gap >= pup_1_l.shape[1]:
        return None
    if y1 + gap >= pup_1_l.shape[0]:
        return None
    # remove_flick = True

    pupil = np.array(pup_1_l)
    c1 = pupil[y1:y1 + gap, x1:x1 + gap].mean()
    c2 = pupil[y2 - gap:y2, x1:x1 + gap].mean()
    c3 = pupil[y1:y1 + gap, x2 - gap:x2].mean()
    c4 = pupil[y2 - gap:y2, x2 - gap:x2].mean()
    #c1 = c2 = c3 = c4 = np.array([c1, c2, c3, c4]).mean()
    bilin = F.interpolate(torch.tensor([[c1, c3], [c2, c4]])[None, None],
                          size=(int(y2 - y1), int(x2 - x1)), mode='bilinear')[0][0].numpy()
    pupil[y1:y2, x1:x2] = bilin + np.random.randn(y2 - y1, x2 - x1) * pupil[y1:y1 + gap, x1:x1 + gap].std()

    return pupil


class AEDDataset(Dataset):
    def __init__(self, datalist, transform=None, target_transform=None, subset='test'):

        self.all_data = []
        for d in datalist:
            if d['subset'] == subset:
                for eye in ['left', 'right']:
                    if eye in d['processed_eyes'][0]:
                        self.all_data.append({'processed_eyes': [{eye: d1[eye]} for d1 in d['processed_eyes']],
                                              'metadata': d['metadata'],
                                              'subset': d['subset'],
                                              'eye': eye})
        self.eye2idx = {'left': 0, 'right': 1}
        self.angle2idx = {0: 0, 60: 1, 120: 2, 180: 3, 240: 4, 300: 5}
        self.transform = transform
        self.subset = subset
        self.target_transform = target_transform
        self.transforms = v2.Compose([
            v2.ToTensor(),
            v2.RandomVerticalFlip(p=0.5),
            v2.RandomHorizontalFlip(p=0.5),
            # v2.RandomAdjustSharpness(2, p=0.5),
            # v2.GaussianBlur(7),
            # v2.GaussianNoise(sigma=2, clip=),
        ])

    def __len__(self):
        return len(self.all_data)

    @staticmethod
    def collate(batch):
        sz_array = np.array([sz[1].shape for sz in batch])
        b_sz = len(batch)
        w_max, h_max = 128, 128  # sz_array[:, 0].max(), sz_array[:, 1].max()
        place_holder = torch.zeros(b_sz, w_max, h_max)
        mask = torch.zeros(b_sz, w_max, h_max)
        for idx, data in enumerate(batch):
            sh = data[1].shape
            if sh[0] > 128 or sh[1] > 128:
                continue
            w_min = max((w_max - sh[1]) // 2, 0)
            h_min = max((h_max - sh[2]) // 2, 0)
            # TODO replace for noise
            place_holder[idx, w_min:w_min + sh[1], h_min:h_min + sh[2]] = torch.tensor(data[1])[:min(w_max, sh[1]),
                                                                             :min(h_max, sh[2])]
            mask[idx, w_min:w_min + sh[0], h_min:h_min + sh[1]] = 1
        place_holder = (place_holder - 128) / 64
        return place_holder, mask


    def eyelash_emulator(self):
        pass

    def eyelid_emulator(self):
        pass

    def slight_rotation_emulator(self):
        pass

    def flick_change(self, img):
        if np.random.rand() < 0.1:
            flickless_img = remove_flick(img)
            xm, ym = flickless_img.shape
            cx, cy = xm//2, ym//2
            rad = min(cx, cy)
            peak = np.random.randint(128, 255)
            flick = np.zeros_like(img)
            ang, rad = np.random.randint(0, 360),  np.random.randint(0, rad)
            fp_x, fp_y = int(rad * np.cos(ang / 180 * np.pi)), int(rad * np.sin(ang / 180 * np.pi))
            flick[cx + fp_x, cy + fp_y] = 1
            sigma =  [np.random.uniform(0.1, 2.0), np.random.uniform(0.1, 2.0)]
            flick = TVF.gaussian_blur(torch.tensor(flick)[None, None, :,:], [9, 9], sigma)[0,0].numpy()
            flick = flick * peak / flick.max()
            res = flickless_img + flick
            res[res > 255] = 255
            return res
        return img

    def __getitem__(self, idx):
        data = deepcopy(self.all_data[idx])
        eye = data['eye']
        sph, cyl, angle = data['metadata']['sphere'][eye], data['metadata']['cylinder'][eye], \
                          data['metadata']['angle'][eye]
        refr = torch.tensor([sph, cyl, angle])
        random_frame = np.random.choice(np.arange(len(data['processed_eyes'])))
        image = data['processed_eyes'][random_frame][eye]['init_pupil'].astype(np.float32)

        if self.subset == 'train':
            image = self.flick_change(image)
            image = self.transforms(image)[0].numpy()
        return refr, image


class RefDataset(AEDDataset):
    def __init__(self, datalist, transform=None, target_transform=None, subset='train'):
        super().__init__(datalist, transform, target_transform, subset)
        self.eye2idx = {'left': 0, 'right': 1}
        self.pix2mm = 0.09267
        self.all_data = [d for d in self.all_data if
                         4<max(d['processed_eyes'][0][d['eye']]['init_pupil'].shape) * 0.09267 < 8]

    @staticmethod
    def collate(batch):
        sz_array = np.array([sz[1].shape for sz in batch])
        b_sz = len(batch)
        w_max, h_max = 128, 128  # sz_array[:, 0].max(), sz_array[:, 1].max()
        place_holder = torch.zeros(b_sz, 5, w_max, h_max)
        mask = torch.zeros(b_sz, 5, w_max, h_max)
        for idx, data in enumerate(batch):
            sh = data[1].shape
            if sh[0] > 128 or sh[1] > 128:
                continue
            w_min = max((w_max - sh[1]) // 2, 0)
            h_min = max((h_max - sh[2]) // 2, 0)
            # TODO replace for noise
            place_holder[idx, :, w_min:w_min + sh[1], h_min:h_min + sh[2]] = torch.tensor(data[1])[:,
                                                                             :min(w_max, sh[1]),
                                                                             :min(h_max, sh[2])]
            mask[idx, :, w_min:w_min + sh[1], h_min:h_min + sh[2]] = 1
        place_holder = (place_holder - 128) / 64
        rot = torch.tensor([sz[2] for sz in batch])
        eye = torch.tensor([sz[3] for sz in batch])
        return place_holder, mask, rot, eye

    def noise_add(self, img):
        if np.random.random() < 0.3:
            return (torch.tensor(img) + torch.randn_like(torch.tensor(img)) * 4).numpy()
        return img

    def __getitem__(self, idx):
        data = deepcopy(self.all_data[idx])
        eye = data['eye']
        if data['metadata']['start_frame'] in range(1, 41, 4):
            if (data['metadata']['start_frame'] - 1) % 8 == 0:
                rotation = 0  # 0, 60, 120 deg
            else:
                rotation = 1  # 180, 240, 300 deg
        else:
            if data['metadata']['start_frame'] % 8 == 0:
                rotation = 0  # 0, 60, 120 deg
            else:
                rotation = 1  # 180, 240, 300 deg

        eye_n = self.eye2idx[eye]
        imgs = [d[eye]['init_pupil'].astype(np.float32) for d in  data['processed_eyes']]

        shapes = np.array([d.shape for d in  imgs])
        ph = np.zeros((len(imgs), shapes[:, 0].max(), shapes[:, 1].max()))
        # from matplotlib import pyplot as plt

        for idx, im in enumerate(imgs):
            # plt.subplot(1, 5, idx+1)
            # plt.imshow(im)
            ph[idx, :im.shape[0], :im.shape[1]] = im
        # plt.show()
        # if self.subset == 'train':
        #     ph = self.noise_add(ph)
        return 0, ph, rotation, eye_n