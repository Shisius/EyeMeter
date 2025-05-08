import numpy as np
import torch

from rknnlite.api import RKNNLite
from torch.nn import functional as F
from inference_fin import onnx_yolo_pp
from prepare_image import letter_box


class RKNNModel:
    def __init__(self, rknn_model = 'yolov8_seg.rknn',
                        conf=0.5, iou=0.7,
                        imgsz=640):

        self.rknn_model = RKNNLite()
        ret = self.rknn_model.load_rknn(rknn_model)
        if ret != 0:
            print('Load RKNN model failed')
            exit(ret)
        print('done')

        ret = self.rknn_model.init_runtime(core_mask=RKNNLite.NPU_CORE_0)
        self.conf = conf
        self.imgsz = imgsz
        self.iou = iou

    def predict(self, image, save=False, imgsz=640, conf=0.5):
        image = image[-1] if isinstance(image, list) else image
        # data = F.interpolate(torch.tensor(image.astype(np.float32)).permute(2, 0, 1).unsqueeze(0),
        #                      scale_factor=640 / 1936, mode='bilinear')
        # data = torch.cat([140*torch.ones(1, 3, 118, 640), data, 140*torch.ones(1, 3, 121, 640),], dim=2).numpy()
        data = torch.tensor(letter_box(image,  new_shape=(imgsz, imgsz),
                                       auto=False).astype(np.float32)).permute(2, 0, 1).unsqueeze(0).numpy()
        outputs = self.rknn_model.inference(inputs=[data], data_format='nchw')
        inp2 = (data,) + tuple(outputs)
        out = onnx_yolo_pp(inp2, conf=conf if conf is not None else self.conf, iou=self.iou,
                           orig_img=[image])
        return out

    def forwarf(self, image):
        return self.predict(image)

class PupilDetectRKNN:
    def __init__(self, rknn_model = 'yolov8_seg.rknn',
                        conf=0.5, iou=0.7,
                        imgsz=640):
        self.conf = conf
        self.imgsz = imgsz
        self.model = RKNNModel(rknn_model, conf, iou, imgsz)

    def forwarf(self, image):
        return self.model(image)