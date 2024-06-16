#ifndef _ESECAM_MASTER_H_
#define _ESECAM_MASTER_H_

#include <memory>
#include <condition_variable>
#include "cam_types.h"
#include "esecam_print.h"
#include "shmem_alloc.h"
#include "udsunicomm.h"
#include "serial_comm.h"
#include "gpio_tools.h"

#define EYEMETER_N_LEDS 6 
#define EYEMETER_LEDS_MASK ((1 << EYEMETER_N_LEDS) - 1)

#define TRIGGER_GPIO 19

class EseCamMaster
{
public:

	EseCamMaster();
	~EseCamMaster();

	int setup();
	void stop();

	bool is_alive() {return d_is_alive.load();}

	void frame_ready_event(SharedFrame & frame, unsigned char* frame_ptr);
	unsigned short get_led_state() {return d_led_state.load();}

protected:

	PUSB_CAMERA_CAPABILITES d_caps = nullptr;
	std::string d_cam_name;
	StreamSettings d_stream_settings;
	MeasureSettings d_meas_settings;

	std::unique_ptr<ShmemBlockAllocator> d_shmem;
	std::unique_ptr<UdsUniComm> d_uds;
	// std::unique_ptr<SerialComm> d_serial;
	std::vector<std::unique_ptr<SysPwm>> d_irleds;

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

	int start_stream();
	int stop_stream();
	int start_meas();
	void stop_meas();
	int frame_free(UdsUniPack & pack);
	
	int set_led_pwr(UdsUniPack & pack);
	int led_control(unsigned short led_state);

	int set_trigger(bool enable, bool hard);
	int get_frame_soft_trigger();
	int get_frame_hard_trigger();

	void comm_process();
	void meas_routine();
	void cam_timer();

};

#endif // _ESECAM_MASTER_H_
