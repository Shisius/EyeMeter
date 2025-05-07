import socket
import struct
import os
import numpy as np
from multiprocessing import shared_memory

from bin_analyzer import ErrorsEyeMeter
from eyemetercomm import *
import bin_analyzer

UDS_UNI_NAMES_FILEPATH = "/etc/roles.names"
UDS_UNI_SOCK_FOLDER = "/tmp"
UDS_UNI_SOCK_EXT = ".sock"

def udsuni_makesockname(name):
    return UDS_UNI_SOCK_FOLDER + '/' + name + UDS_UNI_SOCK_EXT

class UdsUniCommAI:

    def __init__(self, analyzer=None):
        self.role = EYEMETER_ROLE_AI
        self.sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
        self.name = ''
        self.other_socks = {}
        self.is_alive = False
        self.shframe = shared_memory.SharedMemory(name=FRAME_SHBUF_NAME)
        self.shmeasres = shared_memory.SharedMemory(name=MEAS_RESULT_SHBUF_NAME)
        self.analyzer = bin_analyzer.EyeAnalyzer()
        self.meas_settings = MeasSettings()
        self.meas_result = MeasResult(0,0,0,0,0,0,0,0,0,0,'b',0,0,0,0,0)
        self.shared_frame = SharedFrame()
        self.stream_result = StreamResult(0,0,0,0)
        self.stream_settings = StreamSettings()

    def __del__(self):
        self.is_alive = False
        self.shframe.close()
        self.shmeasres.close()
        self.sock.close()

    def read_roles_file(self):
        with open(UDS_UNI_NAMES_FILEPATH, 'r') as f:
            lines = f.readlines()
            for l in lines:
                datal = l.split(' ')
                if len(datal) < 2:
                    continue
                role = int(datal[0])
                if role == EYEMETER_ROLE_AI:
                    self.name = udsuni_makesockname(datal[1].strip())
                else:
                    self.other_socks.update({role : udsuni_makesockname(datal[1].strip())})

    def setup(self):
        self.read_roles_file()
        if self.name != '':
            try:
                os.unlink(self.name)
            except Exception as e:
                pass
            self.sock.bind(self.name)
            # self.sock.settimeout(0.1)
        else:
            print("UdsUniCommAI: read role file failed!");
        self.is_alive = True

    def send_meas_result(self):
        meas_msg = self.meas_result.pack()
        msg = struct.pack('4B', UDSUNI_PROTO_PTTS4, UDSUNI_TITLE_MEAS_RESULT, UDSUNI_TYPE_MEASURE_RESULT, len(meas_msg))
        msg += meas_msg
        self.sock.sendto(msg, self.other_socks[EYEMETER_ROLE_GUI])

    def share_skew(self, skew_left, skew_right):
        max_n_skew = self.meas_settings.n_repeat * self.meas_settings.n_led_pos
        # set default
        pack_rule = str(max_n_skew*2*2) + 'f'
        self.shmeasres.buf[SHARED_PUPIL_IMAGE_SIZE*2:SHARED_PUPIL_IMAGE_SIZE*2 + struct.calcsize(pack_rule)] = struct.pack(pack_rule, 
            *([MEAS_RESULT_ANGLE_INVAL]*max_n_skew*2*2))
        for i in range(len(skew_left)):
            self.shmeasres.buf[SHARED_PUPIL_IMAGE_SIZE*2 + i*4*2:SHARED_PUPIL_IMAGE_SIZE*2 + i*4*2 + 8] = struct.pack('2f', skew_left[i][0], skew_left[i][1])
        for i in range(len(skew_right)):
            self.shmeasres.buf[SHARED_PUPIL_IMAGE_SIZE*2 + max_n_skew*4*2 + i*4*2:SHARED_PUPIL_IMAGE_SIZE*2 + max_n_skew*4*2 + i*4*2 + 8] = struct.pack('2f', skew_right[i][0], skew_right[i][1])
    
    def share_pupils(self, left_pupil, right_pupil):
        lp_data = np.ndarray([SHARED_PUPIL_IMAGE_WIDTH, SHARED_PUPIL_IMAGE_HEIGHT], dtype=np.uint8, buffer=self.shmeasres.buf)
        lp_data.fill(0)
        rp_data = np.ndarray([SHARED_PUPIL_IMAGE_WIDTH, SHARED_PUPIL_IMAGE_HEIGHT], dtype=np.uint8, buffer=self.shmeasres.buf[SHARED_PUPIL_IMAGE_SIZE:])
        rp_data.fill(0)
        lp_data += -lp_data + left_pupil
        rp_data += -rp_data + right_pupil

    def meas_shoot_done(self):
        print("UdsUniCommAI: shoot done!")
        if self.meas_settings.pixel_bits == 8:
            data = np.ndarray([self.meas_settings.n_led_pos * self.meas_settings.n_repeat, self.meas_settings.frame_height, self.meas_settings.frame_width], 
                              dtype=np.uint8, buffer=self.shframe.buf)
            try:
                out_dict = self.analyzer.process_array(data)
                # print(out_dict)
                # if out_dict.get('error_msg', 'none') != 'none':
                #     raise NotImplementedError  # TODO realize interruption and sending to GUI an error msg
                self.meas_result = MeasResult(out_dict['sph_left'], out_dict['cyl_left'], out_dict['angle_left'], out_dict['left_eye_d'], 
                                            out_dict['sph_right'], out_dict['cyl_right'], out_dict['angle_right'], out_dict['right_eye_d'], 
                                            out_dict['interocular_dist'], out_dict['strabismus'], out_dict['lead_eye'], out_dict['error_msg'],
                                            out_dict['left_sharpness'], out_dict['right_sharpness'], out_dict['left_flick_intensity'], 
                                            out_dict['right_flick_intensity'])
                self.meas_result.add_circle(out_dict['eye_positions']['left_x'], out_dict['eye_positions']['left_y'], out_dict['eye_positions']['left_r'], 
                    out_dict['eye_positions']['right_x'], out_dict['eye_positions']['right_y'], out_dict['eye_positions']['right_r'], out_dict['eye_positions']['n_frame'])
                self.share_skew(out_dict['left_skew'], out_dict['right_skew'])
                try:
                    self.share_pupils(out_dict['left_pupil'], out_dict['right_pupil'])
                except Exception as e:
                    print("Share pupils error: ", e)
                self.send_meas_result()
            except Exception as e:
                print("AI error: ", e)
                # print("\n")
                msg = struct.pack('4B', UDSUNI_PROTO_PTTS4, UDSUNI_TITLE_MEAS_RESULT_FAILED, 0, 0)
                self.sock.sendto(msg, self.other_socks[EYEMETER_ROLE_GUI])
        print("\n")

    def stream_processing(self):
        try:
            if self.stream_settings.pixel_bits == 8:
                data = np.ndarray([self.stream_settings.frame_height, self.stream_settings.frame_width], 
                                  dtype=np.uint8, 
                                  buffer=self.shframe.buf[self.shared_frame.id*self.shared_frame.size:(self.shared_frame.id+1)*self.shared_frame.size])
                out_dict = self.analyzer.process_image(data)
                if 'right_eye_d' in out_dict.keys():
                    self.stream_result = StreamResult(self.shared_frame.id, None, out_dict['left_sharpness'], out_dict['right_sharpness'])
                    self.meas_result.add_circle(out_dict['eye_positions']['left_x'], out_dict['eye_positions']['left_y'], out_dict['eye_positions']['left_r'],
                        out_dict['eye_positions']['right_x'], out_dict['eye_positions']['right_y'], out_dict['eye_positions']['right_r'])
                else:
                    self.stream_result = StreamResult(self.shared_frame.id, out_dict['error_msg'], 0, 0)
                resmsg = self.stream_result.pack()
                msg = struct.pack('4B', UDSUNI_PROTO_PTTS4, UDSUNI_TITLE_FRAME_PROCESSED, UDSUNI_TYPE_STREAM_RESULT, len(resmsg))
                msg += resmsg
                self.sock.sendto(msg, self.other_socks[EYEMETER_ROLE_GUI])
        except Exception as e:
            print("AI stream error: hw", e, self.stream_settings.frame_height, self.stream_settings.frame_width)
            print("AI stream error: range", self.shared_frame.id*self.shared_frame.size, (self.shared_frame.id+1)*self.shared_frame.size)
        msg = struct.pack('4B', UDSUNI_PROTO_PTTS4, UDSUNI_TITLE_FRAME_FREE, UDSUNI_TYPE_INT, struct.calcsize('i'))
        msg += struct.pack('i', self.shared_frame.id)
        self.sock.sendto(msg, self.other_socks[EYEMETER_ROLE_CAM])
        msg = struct.pack('4B', UDSUNI_PROTO_PTTS4, UDSUNI_TITLE_FRAME_BUSY, 0, 0)
        self.sock.sendto(msg, self.other_socks[EYEMETER_ROLE_CAM])

    def recv_process(self):
        while self.is_alive:
            try:
                msg, addr = self.sock.recvfrom(1024)
                _proto, _title, _type, _size = struct.unpack('4B', msg[:4])
                if _proto == 0xAF:
                    if _title == UDSUNI_TITLE_MEAS_RUNNING:
                        print("UdsUniCommAI: Meas settings:", hex(_type), _size)
                        if _type == UDSUNI_TYPE_MEASURE_SETTINGS and _size == struct.calcsize(MEASURE_SETTINGS_RULE):
                            self.meas_settings.unpack(msg[4:])
                            print("UdsUniCommAI: Meas settings:", self.meas_settings.n_led_pos, self.meas_settings.n_repeat)
                            #continue
                        else:
                            print("UdsUniCommAI: Wrong size or type: ", hex(_proto), hex(_title), hex(_type), _size)
                    elif _title == UDSUNI_TITLE_MEAS_SHOOT_DONE:
                        self.meas_shoot_done()
                        #continue
                    elif _title == UDSUNI_TITLE_FRAME_4AI:
                        if _type == UDSUNI_TYPE_SHARED_FRAME and _size == struct.calcsize(SHARED_FRAME_RULE):
                            self.shared_frame.unpack(msg[4:])
                            self.stream_processing()
                    elif _title == UDSUNI_TITLE_STREAM_RUNNING:
                        print("UdsUniCommAI: Stream settings:", hex(_type), _size)
                        if _type == UDSUNI_TYPE_STREAM_SETTINGS and _size == struct.calcsize(STREAM_SETTINGS_RULE):
                            self.stream_settings.unpack(msg[4:])
                            print("UdsUniCommAI: Stream settings:", self.stream_settings.frame_queue_depth, self.stream_settings.pixel_bits)
                            msg = struct.pack('4B', UDSUNI_PROTO_PTTS4, UDSUNI_TITLE_FRAME_BUSY, 0, 0)
                            self.sock.sendto(msg, self.other_socks[EYEMETER_ROLE_CAM])
                            #continue
                        else:
                            print("UdsUniCommAI: Wrong size or type: ", hex(_proto), hex(_title), hex(_type), _size)
                    elif _title == UDSUNI_TITLE_STREAM_START or _title == UDSUNI_TITLE_MEAS_START or _title == UDSUNI_TITLE_FRAME_READY:
                        pass
                    else:
                        print("UdsUniCommAI: Wrong title: ", hex(_proto), hex(_title), hex(_type), _size)
                else:
                    print("UdsUniCommAI: Wrong proto: ", hex(_proto), hex(_title), hex(_type), _size)
            except Exception as e:
                print("UdsUniCommAI: recv failed, ", e)

if __name__ == "__main__":
    ea_inst = bin_analyzer.EyeAnalyzer()
    uc = udsunicomm.UdsUniCommAI(ea_inst)
    uc.setup()
    uc.recv_process()

     
