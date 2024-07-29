import socket
import struct
from multiprocessing import shared_memory
from eyemetercomm import *

UDS_UNI_NAMES_FILEPATH = "/etc/roles.names"
UDS_UNI_SOCK_FOLDER = "/tmp"
UDS_UNI_SOCK_EXT = ".sock"

def udsuni_makesockname(name):
    return UDS_UNI_SOCK_FOLDER + '/' + name + UDS_UNI_SOCK_EXT

class UdsUniCommAI:

    # TITLE_DICT = {UDSUNI_TITLE_MEAS_START: lambda s : s.meas_started()}

    def __init__(self, analyzer):
        self.role = EYEMETER_ROLE_AI
        self.sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
        self.name = ''
        self.other_socks = {}
        self.is_alive = False
        self.shmem = shared_memory.SharedMemory(name=FRAME_SHBUF_NAME)
        self.analyzer = analyzer
        self.meas_settings = MeasSettings()
        self.meas_result = MeasResult(0,0,0,0,0,0,0,0,0)

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
            self.sock.bind(self.name)
            self.sock.settimeout(1)
        else:
            print("UdsUniCommAI: read role file failed!");
        self.is_alive = True

    def send_meas_result(self, title, data):
        msg = struct.pack('4B', UDSUNI_PROTO_PTTS4, UDSUNI_TITLE_MEAS_RESULT, UDSUNI_TYPE_MEASURE_RESULT, 36)
        msg += self.meas_result.pack()
        self.sock.sendto(msg, self.other_socks[EYEMETER_ROLE_GUI])

    def meas_shoot_done(self):
        if self.meas_settings.pixel_bits == 8:
            data = np.ndarray([self.meas_settings.n_led_pos * self.meas_settings.n_repeat, self.meas_settings.frame_height, self.meas_settings.frame_width], 
                              dtype=np.uint8, buffer=self.shmem.buf)
            out_dict = self.analyzer.process_array(data)

    def recv_process(self):
        while self.is_alive:
            try:
                msg, addr = self.sock.recv_from(1024)
                _proto, _title, _type, _size = struct.unpack('4B', msg[:4])
                if _proto == 0xAF:
                    if _title == UDSUNI_TITLE_MEAS_START:
                        if _type == UDSUNI_TYPE_MEASURE_SETTINGS and _size == sturct.calcsize(MEASURE_SETTINGS_RULE):
                            self.meas_settings.unpack(msg[4:])
                            continue
                    if _title == UDSUNI_TITLE_MEAS_SHOOT_DONE:
                        self.meas_shoot_done()
                        continue
                print("UdsUniCommAI: Wrong msg: ", _proto, _title, _type, _size)
            except Exception as e:
                print("UdsUniCommAI: recv failed, ", e)

if __name__ == "__main__":
    ea_inst = bin_analyzer.EyeAnalyzer()
    uc = udsunicomm.UdsUniCommAI(ea_inst)
    uc.setup()
    uc.recv_process()

     
