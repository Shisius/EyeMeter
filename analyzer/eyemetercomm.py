
EYEMETER_ROLE_NONE = 0
EYEMETER_ROLE_GUI = 1
EYEMETER_ROLE_AI = 2
EYEMETER_ROLE_CAM = 3
EYEMETER_ROLE_ALL = 0xFF

FRAME_SHBUF_NAME = "/shframe"
MEASURE_RESULT_RULE = '9f'
MEASURE_SETTINGS_RULE = '3IH6B'

UDSUNI_PROTO_PTTS4 = 0xAF

# Titles
UDSUNI_TITLE_STREAM_START = 0x10
UDSUNI_TITLE_STREAM_RUNNING = 0x12
UDSUNI_TITLE_STREAM_STOP = 0x1A

UDSUNI_TITLE_MEAS_START = 0x20
UDSUNI_TITLE_MEAS_RUNNING = 0x22
UDSUNI_TITLE_MEAS_SHOOT_DONE = 0x24
UDSUNI_TITLE_MEAS_RESULT = 0x26
UDSUNI_TITLE_MEAS_STOP = 0x2A

UDSUNI_TITLE_FRAME_READY = 0x30
UDSUNI_TITLE_FRAME_BUSY = 0x32
UDSUNI_TITLE_FRAME_PROCESSED = 0x34
UDSUNI_TITLE_FRAME_FREE = 0x36

UDSUNI_TITLE_LED_PWR = 0x40


# Types
UDSUNI_TYPE_CHAR = 0x00	# 8 bit. If size > 1 - it is char array
UDSUNI_TYPE_INT = 0x01 # usually 32 bits
UDSUNI_TYPE_FLOAT = 0x2 # float 32

UDSUNI_TYPE_SHARED_FRAME = 0x10
UDSUNI_TYPE_STREAM_SETTINGS = 0x11
UDSUNI_TYPE_MEASURE_SETTINGS = 0x12

UDSUNI_TYPE_MEASURE_RESULT = 0x22

UDSUNI_TYPE_UNKNOWN = 0xFF

class MeasSettings:

	def __init__(self, w = 1936, h = 1216, nbit = 8, nled = 8, repeat = 5):
		self.frame_width = w
		self.frame_height = h
		self.frame_size = w*h*nbit/8
		self.cam_shutter_us = 10
		self.pixel_bits = nbit
		self.cam_format = 0
		self.frame_queue_depth = nled*repeat
		self.fps_max = 30
		self.n_led_pos = nled
		self.n_repeat = repeat

	def unpack(self, msg):
		self.frame_width, self.frame_height, self.frame_size, self.cam_shutter_us, self.pixel_bits, self.cam_format, self.frame_queue_depth, self.fps_max, self.n_led_pos, self.n_repeat = struct.unpack(MEASURE_SETTINGS_RULE, msg)

class EyeParams:

	def __init__(self, s, c, a, d):
		self.sphere = s
		self.cylinder = c
		self.angle = a
		self.diameter = d

	def pack(self):
		return struct.pack('4f', self.sphere, self.cylinder, self.angle, self.diameter)

class MeasResult:

	def __init__(self, ls, lc, la, ld, rs, rc, ra, rd, io):
		self.left = EyeParams(ls, lc, la, ld)
		self.right = EyeParams(rs, rc, ra, rd)
		self.interocular = io

	def pack(self):
		return self.left.pack() + self.right.pack() + struct.pack('f', self.interocular)
	