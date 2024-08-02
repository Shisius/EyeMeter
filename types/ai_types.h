#ifndef _AI_TYPES_H_
#define _AI_TYPES_H_

typedef struct _eye_frame_position
{
	float horiz;
	float vert;
	float width;
	float height;
} EyeFramePos;

typedef struct _eye_circle_position
{
	unsigned short horiz; // in pixels
	unsigned short vert; // in pixels
	float radius; // in pixels
} EyeCirclePos;

typedef struct _AI_stream_result
{
	unsigned int id;

	unsigned char no_face; // > 0 - Ошибка Лицо за кадром
	unsigned char no_eyefix; // > 0 - Ошибка Отсутствие фиксации взгляда
	unsigned char change_distance; // > 0 - Ошибка Слишком близко. < 0 - Ошибка Слишком далеко
	unsigned char reserve; // Ничего не значит пока

	EyeFramePos eye_left;
	EyeFramePos eye_right;
} AIStreamResult;

typedef struct _eye_skew_coords 
{
	float x;
	float y;
} EyeSkewCoords;

typedef struct _eye_meas_parameters
{
	float sphere;
	float cylinder;
	float angle;
	float diameter;
	EyeCirclePos position;
	//unsigned short eye_width;
	//unsigned short eye_height;
	//unsigned int n_skew;
} EyeMeasParameters;

typedef struct _AI_eye_meas_result
{
	EyeMeasParameters left;
	EyeMeasParameters right;
	float interocular;
	unsigned int frame4circles;
} AIEyeMeasResult;

// typedef enum 
// {
// 	LEFT_PUPIL_BLKID = 0,
// 	RIGHT_PUPIL_BLKID,
// 	LEFT_SHEW_BLKID,
// 	RIGHT_SKEW_BLKID,
// 	_AI_MEAS_RESULT_N_BLOCKS
// } AIEyeLargeResultsBlockId;

#endif //_AI_TYPES_H_
