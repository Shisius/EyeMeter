import pickle
import numpy as np
from zernike import RZern
# from pathlib import Path
from tqdm import tqdm
# from PIL import Image
from copy import deepcopy, copy
import torch
from torch.nn import functional as F
from gc import collect
import matplotlib.pyplot as plt

# from src.components.software.primitive_detection.hough_detector.pupil_finder import contour_finder, fit_circle, \
#     circles_find


def get_zernicke_from_image(gray_img, radial_order=6, rad_cut=1.0, norm=False, offset=0):
    L, K = gray_img.shape[0], gray_img.shape[1]
    gray_img = gray_img[offset:L - offset, offset:K - offset]
    cart = RZern(radial_order)
    L, K = gray_img.shape[0], gray_img.shape[1]
    ddx = np.linspace(-rad_cut, rad_cut, K)
    ddy = np.linspace(-rad_cut, rad_cut, L)
    xv, yv = np.meshgrid(ddx, ddy)
    cart.make_cart_grid(xv, yv)

    c0 = np.random.normal(size=cart.nk)
    Phi = np.copy(gray_img)
    if norm:
        Phi = Phi / Phi.mean()
    c1 = cart.fit_cart_grid(np.flipud(Phi))[0]
    return cart, Phi, c1


def process_pupil(pup_1_l):
    flick_pos = np.unravel_index(np.argmax(pup_1_l), np.array(pup_1_l).shape)
    gap = 6
    cut_sz = 12
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
    if True:
        pupil = np.array(pup_1_l)
        c1 = pupil[y1:y1 + gap, x1:x1 + gap].mean()
        c2 = pupil[y2 - gap:y2, x1:x1 + gap].mean()
        c3 = pupil[y1:y1 + gap, x2 - gap:x2].mean()
        c4 = pupil[y2 - gap:y2, x2 - gap:x2].mean()
        #c1 = c2 = c3 = c4 = np.array([c1, c2, c3, c4]).mean()
        bilin = F.interpolate(torch.tensor([[c1, c3], [c2, c4]])[None, None],
                              size=(int(y2 - y1), int(x2 - x1)), mode='bilinear')[0][0].numpy()
        pupil[y1:y2, x1:x2] = bilin + np.random.randn(y2 - y1, x2 - x1) * pupil[y1:y1 + gap, x1:x1 + gap].std()
        # plt.imshow(pupil)
        # plt.show()
    res = get_zernicke_from_image(pupil, offset=0)
    return {'flickless_pupil': pupil,
            'zernicke_c': res[2].astype(np.float32),
            'init_pupil': np.array(pup_1_l),
            'flick_position': flick_pos}


def get_eye_mask(one_eye_m, anoth_eye_m):
    mask_1_r_x, mask_1_r_y = one_eye_m[:, 0], one_eye_m[:, 1]
    mask_1_l_x, mask_1_l_y = anoth_eye_m[:, 0], anoth_eye_m[:, 1]
    pos_l = (int(mask_1_l_y.min()), int(mask_1_l_y.max()), int(mask_1_l_x.min()), int(mask_1_l_x.max()))
    pos_r = (int(mask_1_r_y.min()), int(mask_1_r_y.max()), int(mask_1_r_x.min()), int(mask_1_r_x.max()))
    if pos_l[2] < pos_r[2]:
        pos_l, pos_r = pos_r, pos_l
    return pos_l, pos_r


def speed_estimation(start, end):
    """Determine linear velocity in
    1. First component box center shift
    2. Second component box center shift
    3. Height shift
    4. Width shift"""
    shift_l = (end[0][0] + end[0][1]) / 2 - (start[0][0] + start[0][1]) / 2, \
              (end[0][2] + end[0][3]) / 2 - (start[0][2] + start[0][3]) / 2, \
              (end[0][1] - end[0][0]) - (start[0][1] - start[0][0]), \
              (end[0][3] - end[0][2]) - (start[0][3] - start[0][2])
    shift_r = (end[1][0] + end[1][1]) / 2 - (start[1][0] + start[1][1]) / 2, \
              (end[1][2] + end[1][3]) / 2 - (start[1][2] + start[1][3]) / 2, \
              (end[1][1] - end[1][0]) - (start[1][1] - start[1][0]), \
              (end[1][3] - end[1][2]) - (start[1][3] - start[1][2])
    return shift_l, shift_r


def estimate_coeffs(a, part):
    # outs = pd.model(img)

    start_coord = get_eye_mask(part['start_mask_left'], part['start_mask_right'])
    end_coord = get_eye_mask(part['end_mask_left'], part['end_mask_right'])
    speed_l, speed_r = speed_estimation(start_coord, end_coord)
    result_lst = []
    num_frames = part['end_frame'] - part['start_frame']
    for idx, r in enumerate(range(part['start_frame'] + 1, part['end_frame'], 1)):
        img_1 = a[r]

        x_min_l, x_max_l = start_coord[0][0] + speed_l[0] / num_frames * (idx + 1), \
                           start_coord[0][1] + (speed_l[0] + speed_l[2]) / num_frames * (idx + 1)
        y_min_l, y_max_l = start_coord[0][2] + speed_l[1] / num_frames * (idx + 1), \
                           start_coord[0][3] + (speed_l[1] + speed_l[3]) / num_frames * (idx + 1)

        x_min_r, x_max_r = start_coord[1][0] + speed_r[0] / num_frames * (idx + 1), \
                           start_coord[1][1] + (speed_r[0] + speed_r[2]) / num_frames * (idx + 1)
        y_min_r, y_max_r = start_coord[1][2] + speed_r[1] / num_frames * (idx + 1), \
                           start_coord[1][3] + (speed_r[1] + speed_r[3]) / num_frames * (idx + 1)

        pup_1_l = img_1[int(x_min_l):int(x_max_l), int(y_min_l):int(y_max_l)]
        pup_1_r = img_1[int(x_min_r):int(x_max_r), int(y_min_r):int(y_max_r)]

        left_res = process_pupil(pup_1_l)
        right_res = process_pupil(pup_1_r)
        if left_res is None or right_res is None:
            return None
        result_lst.append({'left': left_res, 'right': right_res})
    return result_lst


def data_process():
    path_to_pickle = 'D:\Projects\eye_blinks\data_24\\12_06_24\\processed.pickle'

    path_to_out_pickle = 'D:\Projects\eye_blinks\data_24\\12_06_24\\refractions.pickle'
    with open(path_to_pickle, 'rb') as f:
        pick = pickle.load(f)
    info_storage = []
    for part in tqdm(pick):
        try:
            fname = part['bin_path']
            with open(fname, 'rb') as opened_file:
                byte_buffer = opened_file.read()
                num_frames = len(byte_buffer) // (1216 * 1936)
                np_buffer = np.ndarray([num_frames, 1216, 1936], dtype=np.uint8, buffer=byte_buffer)
            #np_buffer = np.array(list(np_buffer))
            # np_buffer.setflags(write=1)
            zer_res = estimate_coeffs(np_buffer, part)
            del np_buffer
            del byte_buffer
            collect()
            if zer_res is None:
                continue
            if part['num_exp'] % 10 == 0:
                info_storage.append({'processed_eyes': zer_res,
                                     'metadata': part,
                                     'subset': 'val'})
            else:
                info_storage.append({'processed_eyes': zer_res,
                                     'metadata': part,
                                     'subset': 'train'})

        except:
            pass

    with open(path_to_out_pickle, 'wb') as file:
        # A new file will be created
        pickle.dump(info_storage, file)


if __name__ == '__main__':
    data_process()
