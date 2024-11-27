import torch
from ultralytics import YOLO
from collections import OrderedDict
cfg_root='..\weights\my_yolo8n-seg.yaml'
# path_to_chck = '..\weights\weights.pt'
model = YOLO(cfg_root)
# wab = torch.load(path_to_chck)
# wab = OrderedDict([(k.replace('fc2.', 'model.model.'), v) for k, v in wab.items()])
# model.load_state_dict(wab)
model.model = model.model.eval()
model.fuse()

scripted_module = torch.jit.script(model.model)
frozen_module = torch.jit.freeze(scripted_module)
g = 1