#ifndef _IR_LED_CTL_H_
#define _IR_LED_CTL_H_

#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdatomic.h>
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include <stdint.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include "sdkconfig.h"
#include "led_types.h"
#include "comm_types.h"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
//#define LEDC_OUTPUT_IO          (5) // Define the output GPIO
//#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_10_BIT // Set duty resolution to 13 bits
//#define LEDC_DUTY               (4096) // Set duty to 50%. (2 ** 13) * 50% = 4096
#define LEDC_FREQUENCY          (50000) // Frequency in Hertz. 
#define N_LEDS 6
#define TAG "IR_LED_CTL"
#define TRIGGER_PIN 15
#define GREEN_LED_PIN 40
#define GREEN_LED_CHANNEL LEDC_CHANNEL_6
#define DISPLAY_POWER_PIN 21
#define LED_TASK_PRIO 2
#define CAM_OUT_PIN 38

#define ESP_INTR_FLAG_DEFAULT 0

static int LED_GPIOS[N_LEDS] = {33,16,37,39,18,35}; //{39, 37, 35, 33, 18, 16};

static int LED_CHANNELS[N_LEDS] = {LEDC_CHANNEL_0, LEDC_CHANNEL_1, LEDC_CHANNEL_2, LEDC_CHANNEL_3, LEDC_CHANNEL_4, LEDC_CHANNEL_5};

typedef struct _led_state
{
	uint8_t rgb_blink;
} LedState;

static LedState d_state;
static uint32_t d_frame_number;
static int8_t d_frame_detected;
static uint32_t d_fake_frames;
static uint32_t d_meas_frame;
static uint8_t d_in_meas;
static int8_t d_meas_error;
static int8_t d_reboot_lock;
static SemaphoreHandle_t d_sem_pwm;

static inline int ir_set_duty(int num, float duty)
{
	if (num >=0 && num < N_LEDS && duty >= 0 && duty < 1.0) {
		// Set duty to 50%
		xSemaphoreTake(d_sem_pwm, portMAX_DELAY);
	    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LED_CHANNELS[num], (unsigned int)(round(duty * (float)(1 << 10)))));
	    // Update duty to apply the new value
	    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LED_CHANNELS[num]));
	    xSemaphoreGive(d_sem_pwm);
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

static inline int ir_set_led4frame()
{
	unsigned int i_frame;
	unsigned char i_pos;
	int result = 0;
	if (d_frame_number >= d_meas_frame) {
		i_frame = d_frame_number - d_meas_frame + 1;
		if (i_frame >= LED_DEFAULT_POS_NUM) {
			ir_set_duty_all(0.0);
			d_in_meas = 0;
			return 0;
		}
		i_pos = i_frame % LED_DEFAULT_POS_PER_CYCLE;
		if ((i_pos == 0) || (i_pos == LED_DEFAULT_POS_PER_CYCLE/2)) {
			ir_set_duty_all(LED_DEFAULT_PWM_ALL);
		} else {
			result = ir_set_duty_all(0.0);
			if (i_pos < LED_DEFAULT_POS_PER_CYCLE/2) result -= ir_set_duty( (i_pos - 1), LED_DEFAULT_PWM_ONE );
			else result -= ir_set_duty( (i_pos - 2), LED_DEFAULT_PWM_ONE );
		}
	}
	return result;
}

static inline int disp_power_toggle()
{
	gpio_config_t gpio_conf_dpwr = {
        // config gpios
        .pin_bit_mask = (1ULL << DISPLAY_POWER_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&gpio_conf_dpwr));
	gpio_set_level((gpio_num_t)DISPLAY_POWER_PIN, 1);
	usleep(200000);
	gpio_set_level((gpio_num_t)DISPLAY_POWER_PIN, 0);
	usleep(200000);
	gpio_set_level((gpio_num_t)DISPLAY_POWER_PIN, 1);
	gpio_reset_pin((gpio_num_t)DISPLAY_POWER_PIN);
	gpio_config_t gpio_conf_dpwr1 = {
        // config gpios
        .pin_bit_mask = (1ULL << DISPLAY_POWER_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    // set the gpios as per gpio_conf
    ESP_ERROR_CHECK(gpio_config(&gpio_conf_dpwr1));
    return 0;
}

static inline int timesync_handle(uint8_t * buf)
{
	return 0;
}

static inline int usb_process_rx_buf(uint8_t * buf, size_t * size)
{
	size_t rx_size = *size;
	int result;
	UdsUniMsg msg;
	if (rx_size == 1) {
		uint8_t cmd = buf[0];
		if (cmd == LED_BYTE_CMD_OFF) {
			result = ir_set_duty_all(0.0);
		} else if (cmd == LED_BYTE_CMD_ALL) {
			result = ir_set_duty_all(LED_DEFAULT_PWM_ALL);
		} else if (cmd == LED_BYTE_CMD_BLINK_OFF) {
			d_state.rgb_blink = 0;
			result = 0;
		} else if (cmd == LED_BYTE_CMD_BLINK_ON) {
			d_state.rgb_blink = 1;
			result = 0;
		} else if (cmd == LED_BYTE_CMD_DISP_PWR_ON) {
			result = disp_power_toggle();
		} else if (cmd == LED_BYTE_CMD_TRIG) {
			gpio_set_level((gpio_num_t)TRIGGER_PIN, 1);
			usleep(10);
			gpio_set_level((gpio_num_t)TRIGGER_PIN, 0);
			result = 0;
		} else if (cmd > 0 && cmd <= N_LEDS) {
			if (ir_set_duty_all(0.0) >= 0)
				result = ir_set_duty(cmd - 1, LED_DEFAULT_PWM_ONE);
			else result = -1;
		} else result = -1;
		*size = 1;
		if (result < 0)
			buf[0] = LED_BYTE_ANS_FAIL;
		else
			buf[0] = LED_BYTE_ANS_OK;
	} else if (rx_size >= sizeof(UdsUniMsg)) {
		memcpy(&msg, buf, sizeof(UdsUniMsg));
		if (msg.proto == UDSUNI_PROTO_PTTS4) {
			switch (msg.title) {
			case UDSUNI_TITLE_TIMESYNC:
				result = timesync_handle(buf);
				break;
			case UDSUNI_TITLE_STREAM_START:
				d_fake_frames = 0;
				d_frame_number = 0;
				// d_frame_detected = 0;
				d_in_meas = 0;
				result = 0;
				d_reboot_lock = 1;
				break;
			case UDSUNI_TITLE_MEAS_START:
				if ((msg.type == UDSUNI_TYPE_INT) && (msg.size == sizeof(uint32_t)) && (rx_size >= (sizeof(UdsUniMsg) + sizeof(uint32_t))) && (d_reboot_lock != 0)) {
					memcpy(&d_meas_frame, buf + sizeof(UdsUniMsg), sizeof(uint32_t));
					// d_in_meas = 1;
					*size = sizeof(UdsUniMsg);
					msg.type = 0;
					msg.size = 0;
					memcpy(buf, &msg, sizeof(UdsUniMsg));
					//ir_set_duty_all(LED_DEFAULT_PWM_ALL);
					//d_meas_frame = d_frame_number + 50; // Kludge
					if (d_meas_frame == 1) {
						d_frame_number = 0;
						d_fake_frames = 0;
						// d_frame_detected = 0;
						result = 0;
					} else {
						if (d_meas_frame < d_frame_number + 1) result = -1;
						else result = 0;
					}
					d_in_meas = 1;
					d_meas_error = 0;
				} else result = -1;
				break;
			case UDSUNI_TITLE_MEAS_STOP:
				d_in_meas = 0;
				*size = sizeof(UdsUniMsg) + sizeof(uint32_t);
				if (d_meas_error == 0) msg.type = UDSUNI_TYPE_INT;
				else msg.type = UDSUNI_TYPE_ERROR_GENERIC;
				msg.size = sizeof(uint32_t);
				memcpy(buf, &msg, sizeof(UdsUniMsg));
				memcpy(buf+sizeof(UdsUniMsg), &d_frame_number, sizeof(uint32_t));
				result = 0;
				break;
			default:
				result = -1;
			}
		} else {
			result = -1;
		}
		if (result < 0) {
			msg.proto = UDSUNI_PROTO_PTTS4;
			msg.type = UDSUNI_TYPE_ERROR_GENERIC;
			msg.size = 0;
			*size = sizeof(UdsUniMsg);
			memcpy(buf, &msg, sizeof(UdsUniMsg));
		}
	} else {
		*size = 1;
		buf[0] = LED_BYTE_ANS_FAIL;
	}
	return 0;
}

#endif //_IR_LED_CTL_H_
