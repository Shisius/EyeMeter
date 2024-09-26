import torch
from torch import nn
from ultralytics.utils.tal import TORCH_1_10

from analyzer.postrpocessing_seg import postprocess


class DFL(nn.Module):
    """
    Integral module of Distribution Focal Loss (DFL).

    Proposed in Generalized Focal Loss https://ieeexplore.ieee.org/document/9792391
    """

    def __init__(self, c1=16):
        """Initialize a convolutional layer with a given number of input channels."""
        super().__init__()
        self.conv = nn.Conv2d(c1, 1, 1, bias=False).requires_grad_(False)
        x = torch.arange(c1, dtype=torch.float)
        self.conv.weight.data[:] = nn.Parameter(x.view(1, c1, 1, 1))
        self.c1 = c1

    def forward(self, x):
        """Applies a transformer layer on input tensor 'x' and returns a tensor."""
        b, _, a = x.shape  # batch, channels, anchors
        return self.conv(x.view(b, 4, self.c1, a).transpose(2, 1).softmax(1)).view(b, 4, a)
        # return self.conv(x.view(b, self.c1, 4, a).softmax(1)).view(b, 4, a)


def dist2bbox(distance, anchor_points, xywh=True, dim=-1):
    """Transform distance(ltrb) to box(xywh or xyxy)."""
    lt, rb = distance.chunk(2, dim)
    x1y1 = anchor_points - lt
    x2y2 = anchor_points + rb
    if xywh:
        c_xy = (x1y1 + x2y2) / 2
        wh = x2y2 - x1y1
        return torch.cat((c_xy, wh), dim)  # xywh bbox
    return torch.cat((x1y1, x2y2), dim)  # xyxy bbox


def make_anchors(feats, strides, grid_cell_offset=0.5):
    """Generate anchors from features."""
    anchor_points, stride_tensor = [], []
    assert feats is not None
    dtype, device = feats[0].dtype, feats[0].device
    for i, stride in enumerate(strides):
        _, _, h, w = feats[i].shape
        sx = torch.arange(end=w, device=device, dtype=dtype) + grid_cell_offset  # shift x
        sy = torch.arange(end=h, device=device, dtype=dtype) + grid_cell_offset  # shift y
        sy, sx = torch.meshgrid(sy, sx, indexing="ij") if TORCH_1_10 else torch.meshgrid(sy, sx)
        anchor_points.append(torch.stack((sx, sy), -1).view(-1, 2))
        stride_tensor.append(torch.full((h * w, 1), stride, dtype=dtype, device=device))
    return torch.cat(anchor_points), torch.cat(stride_tensor)


def decode_bboxes(bboxes, anchors, end2end=False):
    """Decode bounding boxes."""
    return dist2bbox(bboxes, anchors, xywh=not end2end, dim=1)

def _inference(x, stride=[8, 16, 32], shape_0=torch.Size([136, 64, 84, 56]),
               device='cpu', no=65, reg_max=16, nc=1):

    """Decode predicted bounding boxes and class probabilities based on multiple-level feature maps."""
    # no is [boxes + 4 + classes]
    stride = torch.tensor(stride).to(device)
    x = [x1.to(device) for x1 in x]
    # Inference path
    shape = x[0].shape  # BCHW
    x_cat = torch.cat([xi.view(shape[0], no, -1) for xi in x], 2)
    # TODO do once!!
    anchors, strides = (x.transpose(0, 1) for x in make_anchors(x, stride, 0.5))
    dfl = DFL(reg_max) if reg_max > 1 else nn.Identity()
    # TODO end do it once

    box, cls = x_cat.split((reg_max * 4, nc), 1)
    dbox = decode_bboxes(dfl(box), anchors.unsqueeze(0)) * strides

    return torch.cat((dbox, cls), 1)  #torch.cat((dbox, cls.sigmoid()), 1)

def io_adapter(x, mc, p):
    return (torch.cat([x[0], mc], 1), (x[1], mc, p))

def onnx_yolo_pp(onnx_out, names={0: 'pupil'}, conf=0.5, iou=0.7):
    x1 = torch.cat([torch.tensor(onnx_out[1]), torch.tensor(onnx_out[2])], dim=1)
    x2 = torch.cat([torch.tensor(onnx_out[5]), torch.tensor(onnx_out[6])], dim=1)
    x3 = torch.cat([torch.tensor(onnx_out[9]), torch.tensor(onnx_out[10])], dim=1)
    out = _inference([x1, x2, x3], )
    masks = torch.cat([torch.tensor(onnx_out[4]).view(1, 32, -1),
                       torch.tensor(onnx_out[8]).view(1, 32, -1),
                       torch.tensor(onnx_out[12]).view(1, 32, -1)], dim=-1)
    proto = torch.tensor(onnx_out[13])
    img = torch.tensor(onnx_out[0])
    in_postpr = io_adapter([out, [x1, x2, x3]], masks, proto)
    result = postprocess(in_postpr, img, (img*255).to(torch.uint8), names=names, conf=conf, iou=iou)
    return result


if __name__ == '__main__':
    import pickle
    from matplotlib import pyplot as plt
    import matplotlib.patches as patches

    with open("C:\\Users\\tomil\\Downloads\\Telegram Desktop\\output (5).pkl", 'rb') as f:
        inp2 = pickle.load(f)

    # inp ~ [[[1, 116, 6720], {inp2[3], inp2[7] inp2[11]}], [1,32,6720], inp2[12]], img, [img_orig]]
    out = onnx_yolo_pp(inp2)
    fig, ax = plt.subplots()
    ax.imshow(out[-1].orig_img.detach().cpu().numpy())
    for b in out[-1].boxes:
        x, y, width, height = (int(b.xywh[0, 0].item()), int(b.xywh[0, 1].item()),
                               int(b.xywh[0, 2].item()), int(b.xywh[0, 3].item()))
        patch = patches.Rectangle((x, y), width, height, facecolor='none', edgecolor='red', linewidth=10)
        ax.add_patch(patch)
    plt.show()
    g = 1
