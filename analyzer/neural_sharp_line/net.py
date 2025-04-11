import torch
from torch import nn

def seq_mask(lens):
    max_len = max(lens)
    cntr = torch.arange(max_len)[None, :]
    mask = cntr.repeat((len(lens), 1))
    return mask < lens[:, None]

class SharpNet(nn.Module):
    def __init__(self):
        super().__init__()
        self.net = nn.Sequential(
            nn.Conv1d(1, 8, kernel_size=3, padding=1),
            nn.BatchNorm1d(8),
            nn.ReLU(),
            nn.Conv1d(8, 16, kernel_size=3, padding=1),
            nn.BatchNorm1d(16),
            nn.ReLU(),
            nn.Conv1d(16, 32, kernel_size=3, padding=1),
            nn.BatchNorm1d(32),
            nn.ReLU(),
        )
        self.classifier = nn.Sequential(
            nn.Linear(64, 32),
            nn.Dropout(0.3),
            nn.ReLU(),
            nn.Linear(32, 1)
        )

    def forward(self, signal, lens=None):
        x = self.net(signal[:, None, :].float())
        if lens is not None:
            mask = seq_mask(lens)
            x = x * mask[:, None, :].to(x.device)
        pool = torch.cat([x.mean(2), x.std(2)], 1)
        return self.classifier(pool)

