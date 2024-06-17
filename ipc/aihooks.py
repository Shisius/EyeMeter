import numpy as np
from multiprocessing import shared_memory
from matplotlib import pyplot as plt
import socket
import struct
import eyemetercomm

class SharedFrames:

    def __init__(self):
        self.shmem = shared_memory.SharedMemory(name=FRAME_SHBUF_NAME)
        self.data = np.ndarray([1216, 1936, 40], dtype=np.uint8, buffer=self.shmem.buf)

    def __del__(self):
        self.shmem.close()

# left and right - lists of 4 float: sphere, cylinder, angle, diameter
def ai_hook_send_result(left, right, interoccular):
	uds = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
	uds.bind("/tmp/ai.sock")
	msg = struct.pack('4B', UDSUNI_PROTO_PTTS4, UDSUNI_TITLE_MEAS_RESULT, UDSUNI_TYPE_MEASURE_RESULT, 36)
	params = left + right + [interoccular]
	msg += struct.pack('9f', *params)
	uds.sento(msg, "/tmp/gui.sock")
	uds.close()


# run:
# sf = SharedFrames()
# left, right, interoccular = ai(sf.data)
# ai_hook_send_result(left, right, interoccular)
