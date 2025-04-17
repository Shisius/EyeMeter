#ifndef _COMM_TYPES_H_
#define _COMM_TYPES_H_

#define FRAME_SHBUF_NAME "/shframe"
#define MEAS_RESULT_SHBUF_NAME "/shmeasres"

typedef struct _udsunimsg
{
	unsigned char proto;
	unsigned char title;
	unsigned char type;
	unsigned char size;
} UdsUniMsg;

typedef struct _eyerolestate
{
	unsigned char role;
	unsigned char ready4stream;
	unsigned char ready4meas;
	unsigned char alarm;
} EyeRoleState;

typedef struct _time_sync_ans_abs
{
	unsigned long long recv_time_ns;
	unsigned long long cur_time_ns;
} TimeSyncAnsAbs;

typedef enum
{
	EYEMETER_ROLE_NONE = 0,
	EYEMETER_ROLE_GUI = 1,
	EYEMETER_ROLE_AI = 2,
	EYEMETER_ROLE_CAM = 3,
	EYEMETER_ROLE_ALL = 0xFF
} EyeMeterRoles;

typedef enum
{
	UDSUNI_PROTO_PTTS4 = 0xAF
} UdsUniProto;

typedef enum
{
	UDSUNI_TITLE_STATE_REQ = 0x08,
	UDSUNI_TITLE_MY_STATE = 0x09,
	UDSUNI_TITLE_TIMESYNC = 0x0C,

	UDSUNI_TITLE_STREAM_START = 0x10,
	UDSUNI_TITLE_STREAM_RUNNING = 0x12,
	UDSUNI_TITLE_STREAM_STOP = 0x1A,

	UDSUNI_TITLE_MEAS_START = 0x20,
	UDSUNI_TITLE_MEAS_RUNNING = 0x22,
	UDSUNI_TITLE_MEAS_SHOOT_DONE = 0x24,
	UDSUNI_TITLE_MEAS_RESULT = 0x26,		// AIEyeMeasResult
	UDSUNI_TITLE_MEAS_STOP = 0x2A,
	UDSUNI_TITLE_MEAS_RESULT_FAILED = 0x2E,
	UDSUNI_TITLE_MEAS_FAILED = 0x2F,

	UDSUNI_TITLE_FRAME_READY = 0x30,		// SharedFrame
	UDSUNI_TITLE_FRAME_BUSY = 0x32,
	UDSUNI_TITLE_FRAME_PROCESSED = 0x34,	// AIStreamResult
	UDSUNI_TITLE_FRAME_FREE = 0x36,

	UDSUNI_TITLE_LED_PWR = 0x40
} UdsUniTitle;

typedef enum
{
	UDSUNI_TYPE_CHAR = 0x00,	// 8 bit. If size > 1 - it is char array
	UDSUNI_TYPE_INT = 0x01,		// usually 32 bits
	UDSUNI_TYPE_FLOAT = 0x02,	// float 32

	UDSUNI_TYPE_SHARED_FRAME = 0x10,
	UDSUNI_TYPE_STREAM_SETTINGS = 0x11,
	UDSUNI_TYPE_MEASURE_SETTINGS = 0x12,

	UDSUNI_TYPE_MEASURE_RESULT = 0x22,

	UDSUNI_TYPE_ROLE_STATE = 0x80,
	UDSUNI_TYPE_TIMESYNC_ANS_ABS = 0x8C,

	UDSUNI_TYPE_ERROR_GENERIC = 0xEE,

	UDSUNI_TYPE_UNKNOWN = 0xFF
} UdsUniType;

#endif //_COMM_TYPES_H_
