#ifndef _ESECAM_MASTER_H_
#define _ESECAM_MASTER_H_

#include <memory>
#include <condition_variable>
#include "cam_types.h"
#include "led_types.h"
#include "esecam_print.h"
#include "shmem_alloc.h"
#include "eye_shmem.h"
#include "udsunicomm.h"
#include "serial_comm.h"
#include "gpio_tools.h"

#define EYEMETER_N_LEDS 6 
#define EYEMETER_LEDS_MASK ((1 << EYEMETER_N_LEDS) - 1)

#define TRIGGER_GPIO 19

#define LED_SERIAL
#define LED_SERIAL_PORT "/dev/ttyACM0"

typedef enum 
{
	ESECAM_TRIGGER_SOFT = 0,
	ESECAM_TRIGGER_HARD,
	ESECAM_TRIGGER_NONE,
	ESECAM_TRIGGER_SOFT_STREAM
} EseCamShotTrigger;

class EseCamMaster
{
public:

	EseCamMaster();
	~EseCamMaster();

	int setup();
	void stop();

	bool is_alive() {return d_is_alive.load();}

	void frame_ready_event(SharedFrame & frame, unsigned char* frame_ptr);
	void set_frame_number(eseusbcam_unsigned_long_t number) {d_frame_number.store(number);}
	unsigned short get_led_state() {return d_led_state.load();}

protected:

	// settings
	PUSB_CAMERA_CAPABILITES d_caps = nullptr;
	std::string d_cam_name;
	StreamSettings d_stream_settings;
	MeasureSettings d_meas_settings;
	unsigned char d_trigger;

	// Objects
	std::unique_ptr<ShmemBlockAllocator> d_shmem;
	std::unique_ptr<ShmemBlockAllocator> d_shmeasres;
	std::unique_ptr<UdsUniComm> d_uds;
#ifdef LED_SERIAL
	std::unique_ptr<SerialComm> d_serial;
#endif
	std::vector<std::unique_ptr<SysPwm>> d_irleds;

	// Synch
	std::thread d_comm_thread;
	std::thread d_cam_timer_thread;
	std::atomic<bool> d_is_alive;
	std::atomic<bool> d_in_stream;
	std::atomic<bool> d_in_meas;
	std::atomic<unsigned short> d_led_state;
	std::chrono::time_point<std::chrono::high_resolution_clock> d_last_frame_time;

	std::condition_variable d_frame_ready_cond;
	std::mutex d_frame_ready_mut;
	std::atomic<bool> d_frame_ready_flag;
	std::atomic<int> d_meas_frame_idx;
	std::atomic<int> d_n_bad_meas_frames;
	std::atomic<eseusbcam_unsigned_long_t> d_frame_number;
	std::atomic<eseusbcam_unsigned_long_t> d_meas_frame_number;
	std::atomic<int> d_meas_error;

	int start_stream();
	int stop_stream();
	int start_meas();
	void stop_meas();
	int frame_free(UdsUniPack & pack);
	int black_frame(unsigned char* frame_ptr);
	
	int set_led_pwr(UdsUniPack & pack);
	int led_control(unsigned short led_state);
	int rgb_blink(bool turn_on);
	int led_msg(unsigned char title, unsigned int * data_ptr);

	int set_trigger();
	int set_trigger(unsigned char trigger_type);
	int get_frame_soft_trigger();
	int get_frame_hard_trigger();
	int reset_frame_cnt();

	void comm_process();
	void meas_routine();
	void cam_timer();
	bool meas_trigger_cycle();
	bool meas_wait_cycle();

};

#endif // _ESECAM_MASTER_H_
