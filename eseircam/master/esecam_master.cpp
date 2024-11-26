
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
				//printf("Esecam callback: %u, %u, %u, %u, %u\n", rRetSrv2.StreamServData.ulNTP_SecondExposureStart, 
				//	rRetSrv2.StreamServData.ulNTP_PartSecondExposureStart, 
				//	rRetSrv2.StreamServData.ulNTP_SecondExposureEnd, rRetSrv2.StreamServData.ulNTP_PartSecondExposureEnd, rRetSrv2.StreamServData.ulFrameNum);
				//  printf("frame time %u\n", rRetSrv2.StreamServData.ulNTP_PartSecondExposureStart);
				master->set_frame_number(rRetSrv2.StreamServData.ulFrameNum);
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

	d_meas_settings.n_led_pos = LED_DEFAULT_POS_PER_CYCLE;
	d_meas_settings.n_repeat = LED_DEFAULT_CYCLE_NUM;

	d_stream_settings.cam_format = 0;
	d_stream_settings.cam_shutter_us = 10000;
	d_stream_settings.frame_queue_depth = d_meas_settings.n_led_pos * d_meas_settings.n_repeat;
	d_stream_settings.fps_max = 30;

#ifdef ESECAM_90HZ
	d_trigger = ESECAM_TRIGGER_NONE; //ESECAM_TRIGGER_SOFT;
#else
	d_trigger = ESECAM_TRIGGER_SOFT;
#endif

	d_uds = std::make_unique<UdsUniComm>(EYEMETER_ROLE_CAM);
#ifdef LED_SERIAL
	d_serial = std::make_unique<SerialComm>(LED_SERIAL_PORT, B115200, 100);
#else
	d_irleds.resize(EYEMETER_N_LEDS);
	for (int iled = 0; iled < EYEMETER_N_LEDS; iled++) {
		d_irleds[iled] = std::make_unique<SysPwm>(iled);
	}
#endif
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

#ifdef LED_SERIAL
	if (d_serial->setup(true) < 0) return -1;
#else
	for (auto & led : d_irleds) {
		if (led->setup() < 0) return -1;
	}
#endif
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

	d_shmeasres = std::make_unique<ShmemBlockAllocator>(MEAS_RESULT_SHBUF_SIZE, 1, MEAS_RESULT_SHBUF_NAME);
	if (d_shmeasres->setup() < 0) return -1;

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

	set_trigger();

	if (reset_frame_cnt() < 0) {
		printf("EseCamMaster:: reset frame cnt failed!\n");
		return -1;
	}

	// Send start stream cmd to esp
	if (d_trigger == ESECAM_TRIGGER_NONE) {
		if (led_msg(UDSUNI_TITLE_STREAM_START, nullptr) < 0) {
			printf("EseCamMaster:: reset frame cnt for MCU failed!\n");
			return -1;
		}
	}
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

	d_meas_error.store(0);
	d_meas_frame_number.store(0);
	d_meas_frame_idx.store(d_meas_settings.n_led_pos * d_meas_settings.n_repeat + 1);
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

	rgb_blink(true);

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

	d_meas_frame_number.store(d_frame_number.load() + 20);
	d_meas_frame_idx.store(0);
	d_n_bad_meas_frames.store(0);

	switch (d_trigger)
	{
	case ESECAM_TRIGGER_SOFT:
	case ESECAM_TRIGGER_HARD:
		meas_failed = meas_trigger_cycle();
		break;
	case ESECAM_TRIGGER_NONE:
		meas_failed = meas_wait_cycle();
		break;
	}

	// d_in_meas.store(false);

	rgb_blink(false);

	std::this_thread::sleep_for(std::chrono::milliseconds(200)); // ??? 1000 ??? CHECK

	// printf("EseCamMaster::black frames number = %d\n", d_n_bad_meas_frames.load());

	if (meas_failed) {
		d_uds->send(UDSUNI_TITLE_MEAS_FAILED);
		printf("EseCamMaster:: meas failed!\n");
	}
	else {
		d_uds->send(UDSUNI_TITLE_MEAS_SHOOT_DONE);
		// if (system("sh /home/orangepi/EyeMeter/run_eye_ai.sh") < 0)
		// 	printf("EseCamMaster:: ai script failed\n");
		printf("EseCamMaster:: meas finished\n");
	}
}

bool EseCamMaster::meas_trigger_cycle()
{
	bool meas_failed = false;
	while (d_meas_frame_idx.load() < d_meas_settings.n_repeat*d_meas_settings.n_led_pos)
	{
		unsigned char i_led = d_meas_frame_idx.load() % d_meas_settings.n_led_pos;
		unsigned char i_repeat = floor(d_meas_frame_idx.load() / d_meas_settings.n_led_pos);
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
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
			if (d_frame_ready_flag.load()) {
				// std::this_thread::sleep_for(std::chrono::milliseconds(15));
				break;
			} else {
				printf("EseCamMaster::meas frame %d, %d repeat\n", i_repeat, i_led);
				stop_meas();
				break;
			}
		}
		if (d_n_bad_meas_frames.load() > 4) {
			printf("EseCamMaster:: %d bad frames detected\n", d_n_bad_meas_frames.load());
			stop_meas();
			break;
		}
		d_meas_frame_idx++;
	}
	if (!d_in_meas.load()) meas_failed = true;
	stop_stream();
	return meas_failed;
}

bool EseCamMaster::meas_wait_cycle()
{
	bool meas_failed = false;
	int n_wait = d_meas_settings.n_repeat*d_meas_settings.n_led_pos*2;
	int i_wait = 0;
	int wait_time_ms = 11;
	// Send start cmd to esp
	unsigned int meas_frame_number = d_meas_frame_number.load();
	if (led_msg(UDSUNI_TITLE_MEAS_START, &meas_frame_number) < 0) {
		printf("EseCamMaster: send UDSUNI_TITLE_MEAS_START to MCU failed\n");
		i_wait = n_wait;
		meas_failed = true;
	}
	while (d_in_meas.load() && (i_wait < n_wait)) {
		std::this_thread::sleep_for(std::chrono::milliseconds(wait_time_ms));
		i_wait++;
		if (d_meas_frame_idx.load() >= d_meas_settings.n_repeat*d_meas_settings.n_led_pos) {
			break;
		}
	}

	/*
	TRIGGER_CTRL_REG reg;

	if (USB_GetCameraFeature(d_cam_name.c_str(), TRIGGER_MODE, (eseusbcam_unsigned_long_t*)&reg) != 1)
		printf("EseCamMaster:: get trigger failed\n");

	reg.OnOff = 1;
	reg.TriggerMode = 3;

	eseusbcam_unsigned_long_t _reg;
	memcpy(&_reg, &reg, sizeof(TRIGGER_CTRL_REG));
	if (USB_SetCameraFeature(d_cam_name.c_str(), TRIGGER_MODE, _reg) != 1)
		printf("EseCamMaster:: set trigger failed\n");

	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	*/

	stop_stream();
	if (i_wait >= n_wait) meas_failed = true;
	if (led_msg(UDSUNI_TITLE_MEAS_STOP, &meas_frame_number) < 0) {
		printf("EseCamMaster: send UDSUNI_TITLE_MEAS_STOP to MCU failed\n");
		meas_failed = true;
	} else {
		printf("EseCamMaster: MCU frame sync result cpu = %u, mcu = %u\n", d_frame_number.load(), meas_frame_number);
		if (meas_frame_number > (d_frame_number.load() + 1)) {
			printf("EseCamMaster: MCU frame sync failed cpu = %u, mcu = %u\n", d_frame_number.load(), meas_frame_number);
			meas_failed = true;
		}
	}
	return meas_failed;
}

int EseCamMaster::stop_stream()
{
	USB_StopVideoStream(d_cam_name.c_str());
	d_in_stream.store(false);
	d_in_meas.store(false);
	printf("EseCamMaster:: stop stream\n");
	//if (d_trigger != ESECAM_TRIGGER_NONE) 
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

int EseCamMaster::rgb_blink(bool turn_on)
{
	// return 0; //Kludge
#ifdef LED_SERIAL
	unsigned char cmd;
	if (turn_on) {
		cmd = LED_BYTE_CMD_BLINK_ON;
	} else {
		cmd = LED_BYTE_CMD_BLINK_OFF;
	}

	if (d_serial->write_from((char*)&cmd, 1) > 0) {
		printf("EseCamMaster:: led send cmd %d\n", cmd);
	} else {
		printf("EseCamMaster:: led send cmd %d failed!!!\n", cmd);
		return -1;
	}

	unsigned char ans = 0;
	if (d_serial->read_to((char*)&ans, 1) <= 0) {
		printf("EseCamMaster::led control no answer\n");
		return -1;
	}
	if (ans != LED_BYTE_ANS_OK) {
		printf("EseCamMaster::led control failed with ans %x for cmd %x\n", ans, cmd);
		return -1;
	}
#endif
	return 0;
}

int EseCamMaster::led_control(unsigned short led_state)
{

#ifdef LED_SERIAL
	unsigned char cmd;

	if (led_state == 0) cmd = LED_BYTE_CMD_OFF;
	else if (led_state == EYEMETER_LEDS_MASK) cmd = LED_BYTE_CMD_ALL;
	else {
		cmd = 0x00;
		while (led_state > 0) {
			led_state = led_state >> 1;
			cmd += 1;
		}
	}

	if (d_serial->write_from((char*)&cmd, 1) > 0) {
		printf("EseCamMaster:: led send cmd %d\n", cmd);
	} else {
		printf("EseCamMaster:: led send cmd %d failed!!!\n", cmd);
		return -1;
	}

	unsigned char ans = 0;
	if (d_serial->read_to((char*)&ans, 1) <= 0) {
		printf("EseCamMaster::led control no answer\n");
		return -1;
	}
	if (ans != LED_BYTE_ANS_OK) {
		printf("EseCamMaster::led control failed with ans %x for cmd %x\n", ans, cmd);
		return -1;
	}
#else 
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
#endif 

	printf("EseCamMaster:: led set %d\n", led_state);

	d_led_state.store(led_state);

	return 0;
}

int EseCamMaster::led_msg(unsigned char title, unsigned int * data_ptr)
{
	UdsUniMsg msg;
	UdsUniMsg ans;

#ifdef LED_SERIAL

	msg.proto = UDSUNI_PROTO_PTTS4;
	msg.title = title;
	msg.type = UDSUNI_TYPE_INT;
	if (title == UDSUNI_TITLE_MEAS_START)
		msg.size = sizeof(unsigned int);
	else msg.size = 0;

	char buf[sizeof(UdsUniMsg) + msg.size];
	memcpy(buf, &msg, sizeof(UdsUniMsg));
	if (title == UDSUNI_TITLE_MEAS_START)
		memcpy(buf+sizeof(UdsUniMsg), data_ptr, sizeof(unsigned int));
	
	if (d_serial->write_from(buf, sizeof(UdsUniMsg) + msg.size) > 0) {
		printf("EseCamMaster:: led send title %x, size = %d\n", msg.title, msg.size);
	} else {
		printf("EseCamMaster:: led send title %x failed!!!\n", msg.title);
		return -1;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	
	if (d_serial->read_to((char*)&ans, sizeof(UdsUniMsg)) <= 0) {
		printf("EseCamMaster::led msg no answer for title %x\n", msg.title);
		return -1;
	}
	printf("EseCamMaster::led ans = %x, %x, %x, %x\n", ans.proto, ans.title, ans.type, ans.size);
	if (ans.proto == UDSUNI_PROTO_PTTS4) {
		if (ans.title != msg.title) {
			printf("EseCamMaster::led msg title error cmd = %x, ans = %x \n", msg.title, ans.title);
			return -1;
		}
		if (ans.size == sizeof(unsigned int)) {
			if (d_serial->read_to((char*)(data_ptr), sizeof(unsigned int)) <= 0) {
				printf("EseCamMaster::led msg no data for title %x\n", ans.title);
				return -1;
			}
			printf("EseCamMaster: led data %u for title %x, type %x\n", *data_ptr, ans.title, ans.type);
		}
		if (ans.type == UDSUNI_TYPE_ERROR_GENERIC) {
			printf("EseCamMaster: led ans error detected\n");
			return -1;
		}
	} else {
		printf("EseCamMaster: led ans wrong proto %x\n", ans.proto);
		return -1;
	}

#endif 

	return 0;	
}

int EseCamMaster::set_trigger()
{
	bool enable = false;
	bool hard = false;
	if (d_trigger != ESECAM_TRIGGER_NONE) {
		enable = true;
		if (d_trigger == ESECAM_TRIGGER_HARD)
			hard = true;
	}

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
#ifdef LED_SERIAL
	unsigned char cmd = LED_BYTE_CMD_TRIG;
	if (d_serial->write_from((char*)&cmd, 1) > 0) {
		printf("EseCamMaster:: led send cmd %d\n", cmd);
	} else {
		printf("EseCamMaster:: led send cmd %d failed!!!\n", cmd);
		return -1;
	}

	unsigned char ans = 0;
	if (d_serial->read_to((char*)&ans, 1) <= 0) {
		printf("EseCamMaster::led control no answer\n");
		return -1;
	}
	if (ans != LED_BYTE_ANS_OK) {
		printf("EseCamMaster::led control failed with ans %x for cmd %x\n", ans, cmd);
		return -1;
	}
#else
	//digitalWrite(TRIGGER_GPIO, LOW);
	std::this_thread::sleep_for(std::chrono::microseconds(100));
	//digitalWrite(TRIGGER_GPIO, HIGH);
#endif
	return 0;
}

int EseCamMaster::reset_frame_cnt()
{
	unsigned int reset = 0;
	if (USB_GetCameraFeature(d_cam_name.c_str(), RST_REG, &reset) != 1) {
		printf("EseCamMaster::get feature failed\n");
		return -1;
	} 
	// printf("Reset = %x\n", reset);
	reset |= FLAG_RST_FRAME_CNT;
	if (USB_SetCameraFeature(d_cam_name.c_str(), RST_REG, reset) != 1) {
		printf("EseCamMaster::set feature failed\n");
		return -1;
	} 
	d_frame_number.store(0);
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
			switch (d_trigger)
			{
			case ESECAM_TRIGGER_SOFT:
				get_frame_soft_trigger();
				break;
			case ESECAM_TRIGGER_HARD:
				get_frame_hard_trigger();
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
	switch (d_trigger)
	{
	case ESECAM_TRIGGER_SOFT:
	case ESECAM_TRIGGER_HARD:
		if (black_frame(frame_ptr) == 0)
		{
			if (d_shmem->block_alloc(block) == 0) {
				frame.id = block.id;
				memcpy(block.ptr, frame_ptr, frame.size);
				d_uds->send(UDSUNI_TITLE_FRAME_READY, frame);
				printf("EseCamMaster:: frame id %d\n", frame.id);
			} else {
				printf("EseCamMaster:: shmem overflow\n");
			}
		} else {
			printf("EseCamMaster:: black frame %d\n", d_meas_frame_idx.load());
			d_meas_frame_idx--;
			d_n_bad_meas_frames++;
		}
		break;
	case ESECAM_TRIGGER_NONE:
		bool alloc_perm = false;
		if (d_in_meas.load()) {
			if ((d_frame_number.load() >= d_meas_frame_number.load()) && 
				(d_meas_frame_idx.load() < d_meas_settings.n_repeat*d_meas_settings.n_led_pos)) {
				d_meas_frame_idx++;
				alloc_perm = true;
			}
		}
		if (alloc_perm || d_in_stream.load()) {
			if (d_shmem->block_alloc(block) == 0) {
				frame.id = block.id;
				memcpy(block.ptr, frame_ptr, frame.size);
				d_uds->send(UDSUNI_TITLE_FRAME_READY, frame);
				printf("EseCamMaster:: frame id %d num %d\n", frame.id, d_frame_number.load());
			} else {
				printf("EseCamMaster:: shmem overflow\n");
			}
		}
		break;
	}
	d_frame_ready_flag.store(true);
	{
		lock_guard<mutex> lk(d_frame_ready_mut);
		d_frame_ready_cond.notify_one();
	}
}

int EseCamMaster::black_frame(unsigned char* frame_ptr)
{
	return 0;
	/*
	unsigned char max = 0;
	if (!d_in_meas.load()) return 0;
	for (unsigned int i = 7*d_meas_settings.stream.frame_size/16; i < d_meas_settings.stream.frame_size/2; i++) {
		if (*(frame_ptr+i) > max) max = *(frame_ptr+i);
	}
	if (max > 90) return 0;
	return 1;
	*/
}
