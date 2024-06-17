
#include "esecam_master.h"

void esecam_callback (const char* szCameraName,
						int eventID,
						int eventDataSize,
						void* pEventData,
		                unsigned char* pFrame,
						void*  pUserData)
{
	EseCamMaster * master = (EseCamMaster*)(pUserData);
	// printf("EseCamMaster::frame callback\n");
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
				frame.led_state = master->get_led_state();
				// printf("Esecam callback: Width = %d, Height = %d, bpp =%d\n", frame.width, frame.height, frame.pixel_bits);
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
	d_in_stream.store(false);
	d_in_meas.store(false);

	d_led_state.store(0);

	d_meas_settings.n_led_pos = 8;
	d_meas_settings.n_repeat = 5;

	d_stream_settings.cam_format = 0;
	d_stream_settings.cam_shutter_us = 10000;
	d_stream_settings.frame_queue_depth = d_meas_settings.n_led_pos * d_meas_settings.n_repeat;
	d_stream_settings.fps_max = 5;

	d_uds = std::make_unique<UdsUniComm>(EYEMETER_ROLE_CAM);
	// d_serial = std::make_unique<SerialComm>("/dev/ttyUSB0", B115200, 100);
	d_irleds.resize(EYEMETER_N_LEDS);
	for (int iled = 0; iled < EYEMETER_N_LEDS; iled++) {
		d_irleds[iled] = std::make_unique<SysPwm>(iled);
	}
}

EseCamMaster::~EseCamMaster()
{
	stop();
}

void EseCamMaster::stop()
{
	stop_stream();
	stop_meas();
	d_is_alive.store(false);
	d_uds->stop();
	if (d_comm_thread.joinable())
		d_comm_thread.join();
}

int EseCamMaster::setup()
{
	if (d_uds->start() < 0) return -1;

	// if (wiringPiSetup() == -1) {
	// 	printf("EseCamMaster: wiringPi setup failed \n");
    // 	return -1;
	// }

	d_cam_name = esecam_getname();
	if (d_cam_name == "") {
		printf("EseCamMaster:: esecam is not found\n");
		return -1;
	}

	// if (d_serial->setup(true) < 0) return -1;
	for (auto & led : d_irleds) {
		if (led->setup() < 0) return -1;
	}
	//pinMode(TRIGGER_GPIO, OUTPUT);
	//digitalWrite(TRIGGER_GPIO, HIGH);

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

	d_meas_settings.stream = d_stream_settings;

	d_shmem = std::make_unique<ShmemBlockAllocator>(d_stream_settings.frame_size, d_stream_settings.frame_queue_depth, FRAME_SHBUF_NAME);
	if (d_shmem->setup() < 0) return -1;

	d_is_alive.store(true);
	d_comm_thread = std::thread(&EseCamMaster::comm_process, this);
	return 0;
}

int EseCamMaster::start_stream()
{
	printf("EseCamMaster:: start stream\n");
	if (d_in_stream.load()) {
		stop_stream();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	// Shmem
	d_shmem->clear();
	d_shmem->resize(d_stream_settings.frame_size, d_stream_settings.frame_queue_depth);
	// Set
	// if (USB_SetFormat(d_cam_name.c_str(), d_stream_settings.cam_format) != 1) {
	// 	printf("EseCamMaster::set format failed\n");
	// 	return -1;
	// }
	// if (USB_SetCameraFeature(d_cam_name.c_str(), SHUTTER, d_stream_settings.cam_shutter_us) != 1) return -1;

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	set_trigger(true, false);

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

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

	// for (int i = 0 ; i < 2; i++) {
	// 	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	// 	get_frame_soft_trigger();
	// }

	d_in_stream.store(true);

	led_control(0xFFFF & EYEMETER_LEDS_MASK);

	d_cam_timer_thread = std::thread(&EseCamMaster::cam_timer, this);

	return 0;
}

int EseCamMaster::start_meas()
{
	if (!d_in_stream.load()) {
		printf("Stream is not started!\n");
		return -1;
	}

	// set_trigger(true, false);

	d_in_stream.store(false);
	d_in_meas.store(true);

	d_meas_settings.stream.frame_queue_depth = d_meas_settings.n_led_pos * d_meas_settings.n_repeat;

	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	meas_routine();

	return 0;

}

void EseCamMaster::meas_routine()
{
	printf("EseCamMaster:: meas started\n");
	bool meas_failed = false;

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	if ((d_shmem->check_free() < d_stream_settings.frame_queue_depth)) {
		printf("EseCamMaster:: waiting shmem %d\n", d_shmem->check_free());
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		d_shmem->clear();
	}	

	if (d_shmem->resize(d_meas_settings.stream.frame_size, d_meas_settings.stream.frame_queue_depth) < 0) {
		printf("EseCamMaster:: shmem resize failed\n");
		d_in_meas.store(false);
	}

	if (d_in_meas.load()) d_uds->send(UDSUNI_TITLE_MEAS_RUNNING, d_meas_settings);

	for (unsigned char i_repeat = 0; i_repeat < d_meas_settings.n_repeat; i_repeat++)
	{
		for (unsigned char i_led = 0; i_led < d_meas_settings.n_led_pos; i_led++) 
		{
			printf("EseCamMaster:: meas %d, %d\n", i_repeat, i_led);
			if (!(d_in_meas.load())) break;
			// Set led
			unsigned short led_value = 0;
			if (i_led == 0 || i_led == d_meas_settings.n_led_pos/2) {
				led_value = 0xFFFF & EYEMETER_LEDS_MASK;
			} else {
				if (i_led < d_meas_settings.n_led_pos/2) led_value = 0x01 << (i_led - 1);
				else led_value = 0x01 << (i_led - 2);
			}
			if (led_control(led_value) < 0) {
				printf("EseCamMaster:: meas failed on led\n");
				stop_meas();
				break;
			}
			// Do shot
			while (d_in_meas.load()) {
				if (get_frame_soft_trigger() < 0) {
					printf("EseCamMaster:: meas failed on trigger\n");
					stop_meas();
					break;
				}
				// get_frame_hard_trigger();
				// Wait
				// {
				// 	unique_lock<mutex> lk(d_frame_ready_mut);
				// 	d_frame_ready_cond.wait(lk,[this]{return (d_frame_ready_flag.load() || !(d_in_meas.load()));});
				// }
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				if (d_frame_ready_flag.load()) {
					// std::this_thread::sleep_for(std::chrono::milliseconds(15));
					break;
				} else {
					printf("EseCamMaster::meas frame %d, %d repeat\n", i_repeat, i_led);
					stop_meas();
					break;
				}
			}
		}
	}

	if (!d_in_meas.load()) meas_failed = true;
	stop_stream();
	// d_in_meas.store(false);

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	if (meas_failed) {
		d_uds->send(UDSUNI_TITLE_MEAS_FAILED);
		printf("EseCamMaster:: meas failed!\n");
	}
	else {
		d_uds->send(UDSUNI_TITLE_MEAS_SHOOT_DONE);
		if (system("sh /home/orangepi/EyeMeter/run_eye_ai.sh") < 0)
			printf("EseCamMaster:: ai script failed\n");
		printf("EseCamMaster:: meas finished\n");
	}
}

int EseCamMaster::stop_stream()
{
	printf("EseCamMaster:: stop stream\n");
	USB_StopVideoStream(d_cam_name.c_str());
	d_in_stream.store(false);
	d_in_meas.store(false);
	led_control(0);
	if (d_cam_timer_thread.joinable())
		d_cam_timer_thread.join();
	return 0;
}

void EseCamMaster::stop_meas()
{
	lock_guard<mutex> lk(d_frame_ready_mut);
	d_in_meas.store(false);
	d_frame_ready_cond.notify_one();
}

int EseCamMaster::frame_free(UdsUniPack & pack)
{
	// printf("EseCamMaster::free %d\n", pack.msg.type);
	int block_id = 0;
	if (pack.msg.type == UDSUNI_TYPE_INT) {
		int id;
		if (pack.fetch_data(id) == 0) {
			block_id = id;
		}
	} else if (pack.msg.type == UDSUNI_TYPE_SHARED_FRAME) {
		SharedFrame sf;
		if (pack.fetch_data(sf) == 0) {
			block_id = sf.id;
		}
	}
	// if (d_in_stream.load())
		d_shmem->block_free(block_id);
	// if (d_in_stream.load()) {
	// 	std::this_thread::sleep_for(std::chrono::milliseconds(30));
	// 	if (d_in_stream.load()) get_frame_soft_trigger();
	// }			
	return 0;
}

int EseCamMaster::set_led_pwr(UdsUniPack & pack)
{
	if (pack.msg.type == UDSUNI_TYPE_INT) {
		int pwr;
		unsigned short led_state;
		if (pack.fetch_data(pwr) == 0) {
			if (pwr == 0) led_state = 0x0000;
			else led_state = 0xFFFF & EYEMETER_LEDS_MASK;
			led_control(led_state);
			return 0;
		}
	}
	return -1;
}

int EseCamMaster::led_control(unsigned short led_state)
{
	// unsigned char cmd;

	// if (led_state == 0) cmd = 0x00;
	// else if (led_state == EYEMETER_LEDS_MASK) cmd = 0x0F;
	// else {
	// 	cmd = 0x00;
	// 	while (led_state > 0) {
	// 		led_state = led_state >> 1;
	// 		cmd += 1;
	// 	}
	// }

	// if (d_serial->write_from((char*)&cmd, 1) > 0) {
	// 	printf("EseCamMaster:: led send cmd %d\n", cmd);
	// } else {
	// 	printf("EseCamMaster:: led send cmd %d failed!!!\n", cmd);
	// 	return -1;
	// }

	// unsigned char ans = 0;
	// if (d_serial->read_to((char*)&ans, 1) <= 0) {
	// 	printf("EseCamMaster::led control no answer\n");
	// 	return -1;
	// }
	// if (ans != cmd) {
	// 	printf("EseCamMaster::led control ans %d != cmd %d\n", ans, cmd);
	// 	return -1;
	// }

	if (led_state == 0) {
		for (auto & led : d_irleds) {
			if (led->set_duty(0.0) < 0) {
				printf("EseCamMaster:: set duty failed\n");
				return -1;
			}
		}
	} else if (led_state == EYEMETER_LEDS_MASK) {
		for (auto & led : d_irleds) {
			if (led->set_duty(0.14) < 0) {
				printf("EseCamMaster:: set duty failed\n");
				return -1;
			}
		}
	} else {
		unsigned int iled = 0;
		unsigned short ledstate_tmp = led_state >> 1;
		while (ledstate_tmp > 0) {
			ledstate_tmp = ledstate_tmp >> 1;
			iled += 1;
		}
		if (iled >= d_irleds.size()) {
			printf("EseCamMaster:: wrong led_state %d\n", led_state);
			return -1;
		}
		if (d_irleds[iled]->set_duty(0.84) < 0) {
			printf("EseCamMaster:: set duty failed\n");
			return -1;
		}
	}

	printf("EseCamMaster:: led set %d\n", led_state);

	d_led_state.store(led_state);

	return 0;
}

int EseCamMaster::set_trigger(bool enable, bool hard)
{
	TRIGGER_CTRL_REG reg;

	if (USB_GetCameraFeature(d_cam_name.c_str(), TRIGGER_MODE, (eseusbcam_unsigned_long_t*)&reg) != 1) {
		printf("EseCamMaster:: get trigger failed\n");
		return -1;
	}

	if (enable) reg.OnOff = 1;
	else reg.OnOff = 0;

	if (hard) reg.TriggerMode = 0;
	else reg.TriggerMode = 3;

	eseusbcam_unsigned_long_t _reg;
	memcpy(&_reg, &reg, sizeof(TRIGGER_CTRL_REG));
	if (USB_SetCameraFeature(d_cam_name.c_str(), TRIGGER_MODE, _reg) == 1) return 0;
	printf("EseCamMaster:: set trigger failed\n");
	return -1;
}

int EseCamMaster::get_frame_soft_trigger()
{
	d_frame_ready_flag.store(false);
	int result = USB_SetCameraFeature(d_cam_name.c_str(), ONE_MULTI_FRAME_REQUEST, 1);
	if (result == 1) {
		return 0;
	}
	printf("EseCamMaster::soft trigger failed\n");
	return -1;
}

int EseCamMaster::get_frame_hard_trigger()
{
	d_frame_ready_flag.store(false);
	//digitalWrite(TRIGGER_GPIO, LOW);
	std::this_thread::sleep_for(std::chrono::microseconds(100));
	//digitalWrite(TRIGGER_GPIO, HIGH);
	return 0;
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
				case UDSUNI_TITLE_MEAS_START:
					start_meas();
					break;
				default:
					break;
			}
		}
		pack.clear_data();
	}
}

void EseCamMaster::cam_timer()
{
	while (d_in_stream.load()) {
		std::this_thread::sleep_for(std::chrono::microseconds(1000000 / d_stream_settings.fps_max));
		if (d_in_stream.load()) {
			get_frame_soft_trigger();
			// get_frame_hard_trigger();
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
		printf("EseCamMaster:: frame id %d\n", frame.id);
	} else {
		printf("EseCamMaster:: shmem overflow\n");
	}
	d_frame_ready_flag.store(true);
	{
		lock_guard<mutex> lk(d_frame_ready_mut);
		d_frame_ready_cond.notify_one();
	}
}
