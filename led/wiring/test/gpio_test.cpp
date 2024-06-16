
#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "iostream"
#include <chrono>

/**
 * PWM  GPIO WPi
 * 0_2	 27  17
 * 1_2 	 28  18
 * 3_3 	 12  6
 * 13_2	 7	 2
 * 14_1	 11  5
 * 14_2	 32  21
 * 15_1	 13  7
 * 15_3	 33  22
 * 
 * 1_A4  29  19
 */
using time_point_ns_t = uint64_t;
using time_point_t = std::chrono::time_point<std::chrono::high_resolution_clock>;
inline time_point_t get_system_time() {return std::chrono::high_resolution_clock::now();}
inline time_point_ns_t get_duration_ns(time_point_t start_time, time_point_t finish_time) 
{
	return static_cast<time_point_ns_t>(duration_ns_t(finish_time - start_time).count());
}
inline void thread_sleep_ns(int delay_ns) {std::this_thread::sleep_for(std::chrono::nanoseconds(delay_ns));}

int main()
{
	// int bright ;

	if (wiringPiSetup () == -1) {
		printf("Setup failed \n");
    	return 1;
	}

  	// pinMode (17, PWM_OUTPUT) ;
  	// pinMode (18, PWM_OUTPUT) ;
  	// pinMode (6, PWM_OUTPUT) ;
  	// pinMode (2, PWM_OUTPUT) ;

  	// pinMode (5, PWM_OUTPUT) ;
  	// pinMode (21, PWM_OUTPUT) ;

  	// pinMode (7, PWM_OUTPUT) ;
  	// pinMode (22, PWM_OUTPUT) ;

  	pinMode (18, OUTPUT) ;

  	// pwmWrite (17, 900) ;
  	// pwmWrite (18, 800) ;

  	// pwmWrite (6, 700) ;
  	// pwmWrite (2, 600) ;

  	// pwmWrite (5, 500) ;
  	// pwmWrite (21, 400) ;

  	// pwmWrite (7, 300) ;
  	// pwmWrite (22, 200) ;

  	// delay (1) ;

  	std::cin.get() ;

  	for (;;)
	{
		time_point_t start_time = get_system_time();
		digitalWrite (19, HIGH) ;	// On
		while (get_duration_ns(start_time, get_system_time()) < 80000) {
			thread_sleep_ns(1000);
		}		// mS
		digitalWrite (19, LOW) ;	// Off
		while (get_duration_ns(start_time, get_system_time()) < 100000) {
			thread_sleep_ns(1000);
		}
	}

	return 0 ;
}
