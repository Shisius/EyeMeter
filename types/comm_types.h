#ifndef _COMM_TYPES_H_
#define _COMM_TYPES_H_

typedef struct _udsunimsg
{
	unsigned char proto;
	unsigned char title;
	unsigned char type;
	unsigned char size;
} UdsUniMsg;

typedef enum
{
	UDSUNI_PROTO_PTTS4 = 0xAF
} UdsUniProto;

typedef enum
{
	UDSUNI_TITLE_FRAME_READY,
	UDSUNI_TITLE_FRAME_BUSY,
	UDSUNI_TITLE_FRAME_FREE
} UdsUniTitle;

typedef enum
{
	UDSUNI_TYPE_CHAR = 0,
	UDSUNI_TYPE_INT = 1,
	UDSUNI_TYPE_FLOAT = 2,

	UDSUNI_TYPE_SHARED_FRAME = 0x10

} UdsUniType;

#endif //_COMM_TYPES_H_
