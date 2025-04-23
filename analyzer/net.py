import torch
from torch import nn
from torch.nn import functional as F


class ResFC(nn.Module):
    def __init__(self, hidden_sz=256, do_rate=0.3):
        super().__init__()
        self.fc1 = nn.Linear(hidden_sz, 16)
        self.fc2 = nn.Linear(16, hidden_sz)
        self.act = nn.LeakyReLU()
        self.do = nn.Dropout(do_rate)
        # self.bn = nn.BatchNorm1d(hidden_sz)
        # self.bn2 = nn.BatchNorm1d(hidden_sz)
        self.alpha = nn.Parameter(torch.tensor(1e-5), requires_grad=True)

    def forward(self, x):
        y = x.clone()
        x = self.fc1(x)
        x = self.do(x)
        x = F.leaky_relu(x, negative_slope=0.05)
        x = self.fc2(x)
        x = x * self.alpha + y
        return x


class RefractionNet(nn.Module):
    def __init__(self, input_sz, num_cls, hidden_sz=256, do_rate=0.3, num_layers=2):
        super().__init__()
        self.fc1 = nn.Sequential(
            nn.Linear(input_sz * 5 + 16 + 2, hidden_sz),
            # nn.BatchNorm1d(hidden_sz),
            nn.SELU()
        )
        self.fc2 = nn.Sequential(
            *[ResFC(hidden_sz, do_rate) for _ in range(num_layers)]
        )
        self.fc3 = nn.Sequential(nn.SELU(),
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


class Encoder(nn.Module):
    def __init__(self, hidden_sz=256, do_rate=0.3):
        super().__init__()
        self.encode = nn.Sequential(
            DownSampler(1, 16), # 64
            DownSampler(16, 32), # 32
            DownSampler(32, 64), # 16
            DownSampler(64, 128),  # 8
            DownSampler(128, 128),  # 4
            nn.MaxPool2d(kernel_size=4)
        )

    def forward(self, image, mask=None):
        y = self.encode(image[:, None, :, :])
        return y[:, :, 0, 0]


class DownSampler(nn.Module):
    def __init__(self, in_ch, out_ch):
        super().__init__()
        self.one_part = nn.Sequential(
            nn.Conv2d(in_ch, out_ch, kernel_size=3, stride=1, padding=1),
            nn.BatchNorm2d(out_ch),
            nn.ReLU(),
        )
        self.another_part = nn.Sequential(
            nn.Conv2d(out_ch, out_ch, kernel_size=3, stride=2, padding=1),
            nn.BatchNorm2d(out_ch),
            nn.ReLU(),
        )

    def forward(self, in_ten):
        x = self.one_part(in_ten)
        return self.another_part(x)


class UpSampler(nn.Module):
    def __init__(self, in_ch, out_ch, kernel_size, stride):
        super().__init__()
        self.one_part = nn.Sequential(
            nn.ConvTranspose2d(in_ch, out_ch, kernel_size=kernel_size, stride=stride),
            nn.BatchNorm2d(out_ch),
            nn.ReLU(),
        )
        self.another_part = nn.Sequential(
            nn.Conv2d(out_ch, out_ch, kernel_size=3, stride=1, padding=1),
            nn.BatchNorm2d(out_ch),
            nn.ReLU(),
        )

    def forward(self, in_ten):
        x = self.one_part(in_ten)
        return self.another_part(x)


class Decoder(nn.Module):
    def __init__(self, hidden_sz=256, do_rate=0.3):
        super().__init__()
        self.decode = nn.Sequential(
            UpSampler(128, 128, kernel_size=4, stride=1),  # 4
            UpSampler(128, 128, kernel_size=2, stride=2),  # 8
            UpSampler(128, 64, kernel_size=2, stride=2),  # 16
            UpSampler(64, 32, kernel_size=2, stride=2),  # 32
            UpSampler(32, 16, kernel_size=2, stride=2),  # 64
            nn.ConvTranspose2d(16, 1, kernel_size=2, stride=2),  # 128
        )

    def forward(self, embd):
        y = self.decode(embd[:, :, None, None])
        return y


class RefAED(nn.Module):
    def __init__(self):
        super().__init__()
        self.encoder = Encoder()
        self.decoder = Decoder()

    def forward(self, image, mask=None):
        embed = self.encoder(image, mask)
        return self.decoder(embed)

class RefClass(nn.Module):
    def __init__(self):
        super().__init__()
        self.encoder = Encoder()
        self.classifier = nn.Sequential(
            nn.Linear(640 + 64, 64),
            nn.BatchNorm1d(64),
            nn.ReLU(),
            nn.Dropout(0.4),
            nn.Linear(64, 16),
            nn.BatchNorm1d(16),
            nn.ReLU(),
            nn.Dropout(0.2),
            nn.Linear(16, 3),
        )
        self.eye_emb = nn.Embedding(2, 32)
        self.rot_emb = nn.Embedding(2, 32)


    def forward(self, image, rot, eye, mask=None):
        with torch.no_grad():

            embed = self.encoder(image.view(-1, 128, 128).contiguous(), mask.view(-1, 128, 128).contiguous()).detach()
            embed = embed.view(-1, 5*128).contiguous()
        r_emb = self.rot_emb(rot)
        e_emb = self.eye_emb(eye)
        return self.classifier(torch.cat([embed, r_emb, e_emb], dim=1))