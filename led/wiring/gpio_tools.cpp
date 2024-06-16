#include "gpio_tools.h"

SysPwm::SysPwm(int pwm_num)
{
	d_sys_pwm_num = pwm_num;
	d_period_ns = 100000;
	std::vector<int> d_pwm_numbs = {5, 7, 1, 0, 6, 3};	
}

SysPwm::~SysPwm()
{
	set_duty(0.0);
	close(d_duty_fd);
}

int SysPwm::setup()
{
	if (d_sys_pwm_num >= 0 && d_sys_pwm_num < pwm_numbs.size())
		d_sys_pwm_num = d_pwm_numbs[d_sys_pwm_num];
	else {
		printf("SysPwm: wrong pwm nnumber %d\n", d_sys_pwm_num);
		return -1;
	}
	std::string filename = "/sys/class/pwm/pwmchip" + std::to_string(d_sys_pwm_num) + "/pwm0/duty_cycle";
	d_duty_fd = open(filename.c_str(), O_RDWR);
	if (d_duty_fd < 0) {
		printf("SysPwm: open file %s failed\n", filename.c_str());
		return -1;
	}

	return 0;
}

int SysPwm::set_duty(float duty)
{
	if (duty < 0 || duty > 1.0) {
		printf("SysPwm:: wrong duty %f\n", duty);
		return -1;
	}
	int sysduty = d_period_ns - static_cast<int>(round(duty * static_cast<float>(d_period_ns)));
	std::string duty_s = std::to_string(sysduty);
	int result = write(d_duty_fd. duty_s.c_str(), duty_s.size());
	if (result != duty_s.size()) {
		printf("SysPwm:: write failed with result = %d\n", result);
		return -1;
	}
	return 0;
}
