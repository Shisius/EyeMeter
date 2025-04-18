
import struct

EYEMETER_ROLE_NONE = 0
EYEMETER_ROLE_GUI = 1
EYEMETER_ROLE_AI = 2
EYEMETER_ROLE_CAM = 3
EYEMETER_ROLE_ALL = 0xFF

FRAME_SHBUF_NAME = "/shframe"
MEAS_RESULT_SHBUF_NAME = "/shmeasres"
MEAS_RESULT_SHBUF_SIZE = 0x40000
SHARED_PUPIL_IMAGE_WIDTH = 256
SHARED_PUPIL_IMAGE_HEIGHT = 256
SHARED_PUPIL_IMAGE_SIZE = (SHARED_PUPIL_IMAGE_WIDTH*SHARED_PUPIL_IMAGE_HEIGHT)
MEAS_RESULT_ANGLE_INVAL = 1000.0
MEASURE_RESULT_RULE = '9f'
MEASURE_SETTINGS_RULE = '3IH10B'

UDSUNI_PROTO_PTTS4 = 0xAF

# Titles
UDSUNI_TITLE_STREAM_START = 0x10
UDSUNI_TITLE_STREAM_RUNNING = 0x12
UDSUNI_TITLE_STREAM_STOP = 0x1A

UDSUNI_TITLE_MEAS_START = 0x20
UDSUNI_TITLE_MEAS_RUNNING = 0x22
UDSUNI_TITLE_MEAS_SHOOT_DONE = 0x24
UDSUNI_TITLE_MEAS_RESULT = 0x26
UDSUNI_TITLE_MEAS_RESULT_FAILED = 0x2E
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

DOMINANT_EYE_NONE = 0
DOMINANT_EYE_LEFT = 1
DOMINANT_EYE_RIGHT = 2
DOMINANT_EYE_BOTH = 3

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
		self.frame_width, self.frame_height, self.frame_size, self.cam_shutter_us, self.pixel_bits, self.cam_format, self.frame_queue_depth, self.fps_max, _uu0, _uu1, self.n_led_pos, self.n_repeat, _uu2, uu3 = struct.unpack(MEASURE_SETTINGS_RULE, msg)

class EyeCirclePos:

	def __init__(self, h, v, r):
		self.horiz = h
		self.vert = v
		self.radius = r

	def pack(self):
		return struct.pack('2Hf', self.horiz, self.vert, self.radius)

class EyeParams:

	def __init__(self, s, c, a, d):
		self.sphere = s
		self.cylinder = c
		self.angle = a
		self.diameter = d
		self.position = EyeCirclePos(0,0,0)

	def pack(self):
		return struct.pack('4f', self.sphere, self.cylinder, self.angle, self.diameter) + self.position.pack()

class MeasResult:

	def __init__(self, ls, lc, la, ld, rs, rc, ra, rd, io, strab, dom, err, lsharp, rsharp, lflick, rflick):
		self.left = EyeParams(ls, lc, la, ld)
		self.right = EyeParams(rs, rc, ra, rd)
		self.interocular = io
		self.frame4circles = 0
		self.strabismus = strab
		self.dominant_eye = DOMINANT_EYE_NONE
		if dom[0] == 'r':
			self.dominant_eye = DOMINANT_EYE_RIGHT
		elif dom[0] == 'l':
			self.dominant_eye = DOMINANT_EYE_LEFT
		elif dom[0] == 'b':
			self.dominant_eye = DOMINANT_EYE_BOTH
		self.error_word = 0
		if type(err) is int:
			if err >= 0:
				self.error_word = 1 << err
		# Dev tresh
		self.lsharp = lsharp
		self.rsharp = rsharp
		self.lflick = lflick
		self.rflick = rflick

	def add_circle(self, lh, lv, lr, rh, rv, rr, fc):
		self.left.position.horiz = lh
		self.left.position.vert = lv
		self.left.position.radius = lr
		self.right.position.horiz = rh
		self.right.position.vert = rv
		self.right.position.radius = rr
		self.frame4circles = fc

	def pack(self):
		return self.left.pack() + self.right.pack() + struct.pack('fIfII', self.interocular, self.frame4circles, 
			self.strabismus, self.dominant_eye, self.error_word) + struct.pack('4f', self.lsharp, self.rsharp, self.lflick, self.rflick)
	