
#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "iostream"

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

int main()
{
	// int bright ;

	if (wiringPiSetup () == -1) {
		printf("Setup failed \n");
    	return 1;
	}

  	pinMode (17, PWM_OUTPUT) ;
  	pinMode (18, PWM_OUTPUT) ;
  	pinMode (6, PWM_OUTPUT) ;
  	pinMode (2, PWM_OUTPUT) ;

  	pinMode (5, PWM_OUTPUT) ;
  	pinMode (21, PWM_OUTPUT) ;

  	pinMode (7, PWM_OUTPUT) ;
  	pinMode (22, PWM_OUTPUT) ;

  	pinMode (19, OUTPUT) ;

  	pwmWrite (17, 900) ;
  	pwmWrite (18, 800) ;

  	pwmWrite (6, 700) ;
  	pwmWrite (2, 600) ;

  	pwmWrite (5, 500) ;
  	pwmWrite (21, 400) ;

  	pwmWrite (7, 300) ;
  	pwmWrite (22, 200) ;

  	delay (1) ;

  	std::cin.get() ;

  	for (;;)
	{
		digitalWrite (19, HIGH) ;	// On
		delay (50) ;		// mS
		digitalWrite (19, LOW) ;	// Off
		delay (1) ;
	}

	return 0 ;
}
