#ifndef _COMM_TYPES_H_
#define _COMM_TYPES_H_

#define FRAME_SHBUF_NAME "/shframe"

typedef struct _udsunimsg
{
	unsigned char proto;
	unsigned char title;
	unsigned char type;
	unsigned char size;
} UdsUniMsg;

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
	USDUNI_TITLE_STREAM_START = 0x10,
	USDUNI_TITLE_STREAM_STOP = 0x11,

	USDUNI_TITLE_MEAS_START = 0x20,
	USDUNI_TITLE_MEAS_SHOOT_DONE = 0x21,
	USDUNI_TITLE_MEAS_RESULT = 0x22,
	USDUNI_TITLE_MEAS_STOP = 0x2F,

	UDSUNI_TITLE_FRAME_READY = 0x30,
	UDSUNI_TITLE_FRAME_BUSY = 0x31,
	UDSUNI_TITLE_FRAME_FREE = 0x32
} UdsUniTitle;

typedef enum
{
	UDSUNI_TYPE_CHAR = 0,
	UDSUNI_TYPE_INT = 1,
	UDSUNI_TYPE_FLOAT = 2,

	UDSUNI_TYPE_SHARED_FRAME = 0x10,

	USDUNI_TYPE_UNKNOWN = 0xFF
} UdsUniType;

#endif //_COMM_TYPES_H_
