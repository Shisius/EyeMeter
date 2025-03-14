import torch
from copy import deepcopy
from random import random
import numpy as np
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
