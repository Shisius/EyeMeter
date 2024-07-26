#ifndef _LEDS_SYS_CTL_
#define _LEDS_SYS_CTL_


#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "iostream"
#include <chrono>
#include <vector>

class LedsSysCtl
{
public:

	LedsSysCtl(std::vector<int> wipi_gpio);
	~LedsSysCtl();
	


protected:



};

#endif // _LEDS_SYS_CTL_
