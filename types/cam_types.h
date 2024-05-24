#ifndef _CAM_TYPES_H_
#define _CAM_TYPES_H_

typedef enum
{
	CAM_PIXEL_GRAY_8 = 0
} CamPixelFormat;

typedef struct _shared_frame
{
	unsigned int id;
	unsigned int width;
	unsigned int height;
	unsigned int size;
	unsigned char format;
	unsigned char busy;
} SharedFrame;

#endif //_CAM_TYPES_H_
