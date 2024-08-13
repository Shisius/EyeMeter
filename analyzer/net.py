import torch
from torch import nn
from torch.nn import functional as F


class ResFC(nn.Module):
    def __init__(self, hidden_sz=256, do_rate=0.3):
        super().__init__()
        self.fc1 = nn.Linear(hidden_sz, hidden_sz)
        self.act = nn.LeakyReLU()
        self.do = nn.Dropout(do_rate)
        self.bn = nn.BatchNorm1d(hidden_sz)
        self.bn2 = nn.BatchNorm1d(hidden_sz)

    def forward(self, x):
        y = x
        x = self.fc1(x)
        x = self.do(x)
        x = self.bn(x)
        x = F.relu(x)
        x = x + y
        return x


class RefractionNet(nn.Module):
    def __init__(self, input_sz, num_cls, hidden_sz=256, do_rate=0.3, num_layers=2):
        super().__init__()
        self.fc1 = nn.Sequential(
            nn.Linear(input_sz * 5 + 16 + 2, hidden_sz),
            nn.BatchNorm1d(hidden_sz),
            nn.ReLU()
        )
        self.fc2 = nn.Sequential(
            *[ResFC(hidden_sz, do_rate) for _ in range(num_layers)]
        )
        self.fc3 = nn.Sequential(nn.BatchNorm1d(hidden_sz),
                                 nn.ReLU(),
                                 nn.Linear(hidden_sz, num_cls))
        self.eye_emb = nn.Embedding(2, 8)
        self.angle_emb = nn.Embedding(6, 8)

    def forward(self, flatten_zernicke, eye_type, ter_type, pupil_rad, flick_rad):
        eye = self.eye_emb(eye_type)
        angle = self.angle_emb(ter_type)
        x = torch.cat([flatten_zernicke.view(flatten_zernicke.size(0), -1), eye, angle,
                       pupil_rad[:, None].to(torch.float32),
                       flick_rad[:, None].to(torch.float32)], dim=-1)
        x = self.fc1(x)
        x = self.fc2(x)
        x = self.fc3(x)
        return x
