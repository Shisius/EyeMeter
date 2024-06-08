#ifndef _CAM_TYPES_H_
#define _CAM_TYPES_H_

typedef enum
{
	CAM_PIXEL_GRAY_8 = 8,
	CAM_PIXEL_GRAY_12 = 12
} CamPixelFormat;

typedef struct _shared_frame
{
	unsigned int id;
	unsigned int width;
	unsigned int height;
	unsigned int size;
	unsigned char pixel_bits;
	unsigned char busy;
	unsigned short led_state;
	unsigned short distance_mm;
} SharedFrame;

typedef struct _stream_settings
{
	unsigned int frame_width;
	unsigned int frame_height;
	unsigned int frame_size;
	unsigned short cam_shutter_us;
	unsigned char pixel_bits;
	unsigned char cam_format;
	unsigned char frame_queue_depth;
	unsigned char fps_max;
} StreamSettings;

typedef struct _measure_settings
{
	StreamSettings stream;
	unsigned char n_led_pos;
	unsigned char n_repeat;
} MeasureSettings;

#endif //_CAM_TYPES_H_
