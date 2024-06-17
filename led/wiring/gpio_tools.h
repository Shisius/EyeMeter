#ifndef _GPIO_TOOLS_H_
#define _GPIO_TOOLS_H_

// #include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include "iostream"
#include <chrono>
#include <vector>
#define USE_MATH_DEFINES
#include <math.h>
#include <string>

class SysPwm
{
public:

	SysPwm(int pwm_num);
	~SysPwm();

	int setup();
	int set_duty(float duty);

protected:

	// numbers
	int d_period_ns;
	int d_sys_pwm_num;
	// std::vector<int> d_pwm_numbs;

	// fd
	//int d_period_fd;
	int d_duty_fd;
	//int d_enable_fd;

	int get_sys_number();


};

#endif // _GPIO_TOOLS_H_
