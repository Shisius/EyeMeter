#ifndef _IR_LED_CTL_H_
#define _IR_LED_CTL_H_

#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include <stdint.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include "sdkconfig.h"
#include "led_types.h"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
//#define LEDC_OUTPUT_IO          (5) // Define the output GPIO
//#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_10_BIT // Set duty resolution to 13 bits
//#define LEDC_DUTY               (4096) // Set duty to 50%. (2 ** 13) * 50% = 4096
#define LEDC_FREQUENCY          (50000) // Frequency in Hertz. Set frequency at 4 kHz
#define N_LEDS 6
#define TAG "IR_LED_CTL"
#define TRIGGER_PIN 15

static int LED_GPIOS[N_LEDS] = {33,16,37,39,18,35}; //{39, 37, 35, 33, 18, 16};

static int LED_CHANNELS[N_LEDS] = {LEDC_CHANNEL_0, LEDC_CHANNEL_1, LEDC_CHANNEL_2, LEDC_CHANNEL_3, LEDC_CHANNEL_4, LEDC_CHANNEL_5};

static inline int ir_set_duty(int num, float duty)
{
	if (num >=0 && num < N_LEDS && duty >= 0 && duty < 1.0) {
		// Set duty to 50%
	    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LED_CHANNELS[num], (unsigned int)(round(duty * (float)(1 << 10)))));
	    // Update duty to apply the new value
	    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LED_CHANNELS[num]));
	    return 0;
	}
	return -1;
}

static inline int ir_set_duty_all(float duty)
{
	for (int i_led = 0; i_led < N_LEDS; i_led++) {
        if (ir_set_duty(i_led, duty) < 0)
        	return -1;
    }
    return 0;
}

static inline int usb_process_rx_buf(uint8_t * buf, size_t * size)
{
	size_t rx_size = *size;
	int result;
	if (rx_size == 1) {
		uint8_t cmd = buf[0];
		if (cmd == LED_BYTE_CMD_OFF) {
			result = ir_set_duty_all(0.0);
		} else if (cmd == LED_BYTE_CMD_ALL) {
			result = ir_set_duty_all(0.16);
		} else if (cmd == LED_BYTE_CMD_TRIG) {
			gpio_set_level((gpio_num_t)TRIGGER_PIN, 1);
			usleep(10);
			gpio_set_level((gpio_num_t)TRIGGER_PIN, 0);
			result = 0;
		} else if (cmd > 0 && cmd <= N_LEDS) {
			if (ir_set_duty_all(0.0) >= 0)
				result = ir_set_duty(cmd - 1, 0.96);
			else result = -1;
		} else result = -1;
		*size = 1;
		if (result < 0)
			buf[0] = LED_BYTE_ANS_FAIL;
		else
			buf[0] = LED_BYTE_ANS_OK;
	}
	return 0;
}

#endif //_IR_LED_CTL_H_
