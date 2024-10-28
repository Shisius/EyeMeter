import numpy as np
import torch
from torch.nn import functional as F
from inference_fin import onnx_yolo_pp
import onnxruntime as ort
from prepare_image import letter_box


class RKNNModel:
    def __init__(self, path_to_onnx = 'C:\\Users\\tomil\Downloads\Telegram Desktop\\my_yolo8n-seg (3).onnx',
                        conf=0.5, iou=0.7,
                        imgsz=640):
        self.ort_sess = ort.InferenceSession(path_to_onnx,
                                        providers=['CPUExecutionProvider'])
        self.model_inputs = self.ort_sess.get_inputs()

        self.conf = conf
        self.imgsz = imgsz
        self.iou = iou

    def predict(self, image, save=False, imgsz=640, conf=None):
        image = image[-1] if isinstance(image, list) else image
        # data = F.interpolate(torch.tensor(image.astype(np.float32)).permute(2, 0, 1).unsqueeze(0),
        #                      scale_factor=640 / 1936, mode='bilinear')
        # data = torch.cat([140*torch.ones(1, 3, 118, 640), data, 114*torch.ones(1, 3, 121, 640),], dim=2).numpy()
        data = torch.tensor(letter_box(image, auto=False).astype(np.float32)).permute(2, 0, 1).unsqueeze(0).numpy()
        outputs = self.ort_sess.run(None, {self.model_inputs[0].name: data.astype(np.float32) / 255})
        #outputs = self.rknn_model.inference(inputs=[data], data_format='nchw')
        inp2 = (data,) + tuple(outputs)
        out = onnx_yolo_pp(inp2, conf=conf if conf is not None else self.conf, iou=self.iou,
                           orig_img=[image])
        return out

    def forwarf(self, image):
        return self.predict(image)

class PupilDetectONNX:
    def __init__(self, path_to_onnx = 'yolov8_seg.rknn',
                        conf=0.5, iou=0.7,
                        imgsz=640):
        self.conf = conf
        self.imgsz = imgsz
        self.model = RKNNModel(path_to_onnx, conf, iou, imgsz)

    def forwarf(self, image):
        return self.model(image)