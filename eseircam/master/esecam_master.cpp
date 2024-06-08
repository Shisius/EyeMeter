
#include "esecam_master.h"

void esecam_callback (const char* szCameraName,
						int eventID,
						int eventDataSize,
						void* pEventData,
		                unsigned char* pFrame,
						void*  pUserData)
{
	EseCamMaster * master = (EseCamMaster*)(pUserData);
	if (eventID == NEW_FRAME || eventID == NEW_FRAME_SYNCHRO_ERROR)
	{
		if (pFrame) {
			RET_SERV_DATA_HEADER rRetHeader = *((RET_SERV_DATA_HEADER *)(pFrame));
			if (rRetHeader.ulServDataType == 3) {
				RET_SERV_DATA_ITK4_0 rRetSrv2 = *((RET_SERV_DATA_ITK4_0 *)(pFrame));
				SharedFrame frame;
				frame.width = rRetSrv2.StreamServData.usWidth;
				frame.height = rRetSrv2.StreamServData.usHeight;
				frame.pixel_bits = rRetSrv2.StreamServData.chBitPerPixel;
				frame.size = frame.width * frame.height * frame.pixel_bits / 8;
				printf("Esecam callback: Width = %d, Height = %d, bpp =%d\n", frame.width, frame.height, frame.pixel_bits);
				master->frame_ready_event(frame, pFrame+sizeof(RET_SERV_DATA_ITK4_0));
			} else printf("Esecam callback: wrong Serv data type!\n");
		} else {
			printf("Esecam callback: wrong frame ptr\n");
		}
	} else {
		printf("Esecam callback Event = %d, size = %d\n", eventID, eventDataSize);
	}
}

EseCamMaster::EseCamMaster()
{
	d_is_alive.store(false);

	d_stream_settings.cam_format = 0;
	d_stream_settings.cam_shutter_us = 10000;
	d_stream_settings.frame_queue_depth = 80;

	d_uds = std::make_unique<UdsUniComm>(EYEMETER_ROLE_CAM);
	d_serial = std::make_unique<SerialComm>("/dev/ttyUSB0", B115200);
}

EseCamMaster::~EseCamMaster()
{
	stop();
}

void EseCamMaster::stop()
{
	USB_StopVideoStream(d_cam_name.c_str());
	d_is_alive.store(false);
	d_uds->stop();
	if (d_comm_thread.joinable())
		d_comm_thread.join();
}

int EseCamMaster::setup()
{
	if (d_uds->start() < 0) return -1;

	d_cam_name = esecam_getname();
	if (d_cam_name == "") return -1;

	if (d_serial->setup(true) < 0) return -1;

	if (USB_GetCameraCapabilites(d_cam_name.c_str(), &d_caps) != 1) {
		printf("EseCamMaster::get caps failed\n");
		return -1;
	}

	if (USB_SetFormat(d_cam_name.c_str(), d_stream_settings.cam_format) != 1) {
		printf("EseCamMaster::set format failed\n");
		return -1;
	}

	d_stream_settings.frame_width = d_caps->CameraFormats.formatsList[d_stream_settings.cam_format].MaxWidth;
	d_stream_settings.frame_height = d_caps->CameraFormats.formatsList[d_stream_settings.cam_format].MaxHeight;
	d_stream_settings.pixel_bits = d_caps->CameraFormats.formatsList[d_stream_settings.cam_format].bitCountBitMap;
	d_stream_settings.frame_size = d_stream_settings.frame_width * d_stream_settings.frame_height * d_stream_settings.pixel_bits / 8;

	d_shmem = std::make_unique<ShmemBlockAllocator>(d_stream_settings.frame_size, d_stream_settings.frame_queue_depth, FRAME_SHBUF_NAME);
	if (d_shmem->setup() < 0) return -1;

	d_is_alive.store(true);
	d_comm_thread = std::thread(&EseCamMaster::comm_process, this);
	return 0;
}

int EseCamMaster::start_stream()
{
	// Shmem
	d_shmem->resize(d_stream_settings.frame_size, d_stream_settings.frame_queue_depth);
	// Set
	if (USB_SetFormat(d_cam_name.c_str(), d_stream_settings.cam_format) != 1) return -1;
	// if (USB_SetCameraFeature(d_cam_name.c_str(), SHUTTER, d_stream_settings.cam_shutter_us) != 1) return -1;

	VIDEO_STREAM_PARAM_EX oStreamParam = {};
	oStreamParam.ppReturnedParams = nullptr;
	oStreamParam.pBuffBitmap = nullptr; 
	oStreamParam.pDirectBuffer = nullptr;
	oStreamParam.pEvent = nullptr;
	if (USB_VideoOnCallbackStart(d_cam_name.c_str(), &oStreamParam, esecam_callback, this) != 1) 
	{
		printf("EseCamMaster:: stream start failed!\n");
		return -1;
	}

	d_uds->send(UDSUNI_TITLE_STREAM_RUNNING, d_stream_settings);

	return 0;
}

int EseCamMaster::stop_stream()
{
	USB_StopVideoStream(d_cam_name.c_str());
	return 0;
}

int EseCamMaster::frame_free(UdsUniPack & pack)
{
	if (pack.msg.type == UDSUNI_TYPE_INT) {
		int id;
		if (pack.fetch_data(id) == 0) {
			d_shmem->block_free(id);
			return 0;
		}
	} else if (pack.msg.type == UDSUNI_TYPE_SHARED_FRAME) {
		SharedFrame sf;
		if (pack.fetch_data(sf) == 0) {
			d_shmem->block_free(sf.id);
			return 0;
		}
	}
	return -1;
}

int EseCamMaster::set_led_pwr(UdsUniPack & pack)
{
	if (pack.msg.type == UDSUNI_TYPE_INT) {
		int pwr;
		char cmd;
		if (pack.fetch_data(pwr) == 0) {
			if (pwr == 0) cmd = 0x00;
			else cmd = 0x02;
			d_serial->write_from(&cmd, 1);
			return 0;
		}
	}
	return -1;
}

int EseCamMaster::get_frame_soft_trigger()
{
	if (USB_SetCameraFeature(d_cam_name.c_str(), ONE_MULTI_FRAME_REQUEST, 1) == 1) return 0;
	return -1;
}

void EseCamMaster::comm_process()
{
	while (d_is_alive.load()) {
		UdsUniPack pack;
		if (d_uds->wait_recv(pack) == 0) {
			switch (pack.msg.title) {
				case UDSUNI_TITLE_STREAM_START:
					start_stream();
					break;
				case UDSUNI_TITLE_STREAM_STOP:
					stop_stream();				
					break;
				case UDSUNI_TITLE_FRAME_FREE:
					frame_free(pack);
					break;
				case UDSUNI_TITLE_LED_PWR:
					set_led_pwr(pack);
					break;
				default:
					break;
			}
		}
	}
}

void EseCamMaster::frame_ready_event(SharedFrame & frame, unsigned char* frame_ptr)
{
	ShmemBlock block;
	if (d_shmem->block_alloc(block) == 0) {
		frame.id = block.id;
		memcpy(block.ptr, frame_ptr, frame.size);
		d_uds->send(UDSUNI_TITLE_FRAME_READY, frame);
	} else {
		printf("EseCamMaster:: shmem overflow\n");
	}
}
