import torch
from copy import deepcopy
from random import random
from torch.utils.data import Dataset


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
        self.all_data = datalist
        self.eye2idx = {'left': 0, 'right': 1}
        self.angle2idx = {0: 0, 60: 1, 120: 2, 180: 3, 240: 4, 300: 5}
        self.transform = transform
        self.target_transform = target_transform

    def __len__(self):
        return len(self.all_data)

    def __getitem__(self, idx):
        data = deepcopy(self.all_data[idx])

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
        return rotation, Zernicke_coef, eye, pupil_rad, flick_rad
