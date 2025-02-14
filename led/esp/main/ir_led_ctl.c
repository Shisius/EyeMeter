
#include "ir_led_ctl.h"

/* Warning:
 * For ESP32, ESP32S2, ESP32S3, ESP32C3, ESP32C2, ESP32C6, ESP32H2, ESP32P4 targets,
 * when LEDC_DUTY_RES selects the maximum duty resolution (i.e. value equal to SOC_LEDC_TIMER_BIT_WIDTH),
 * 100% duty cycle is not reachable (duty cannot be set to (2 ** SOC_LEDC_TIMER_BIT_WIDTH)).
 */

static void ir_led_init(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 4 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    for (int i_led = 0; i_led < N_LEDS; i_led ++) {
        
        // Prepare and then apply the LEDC PWM channel configuration
        ledc_channel_config_t ledc_channel = {
            .speed_mode     = LEDC_MODE,
            .channel        = LED_CHANNELS[i_led],
            .timer_sel      = LEDC_TIMER,
            .intr_type      = LEDC_INTR_DISABLE,
            .gpio_num       = LED_GPIOS[i_led],
            .duty           = 0, // Set duty to 0%
            .hpoint         = 0
        };
        ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    }

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = GREEN_LED_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = GREEN_LED_PIN,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

static uint8_t usb_buf[CONFIG_TINYUSB_CDC_RX_BUFSIZE + 1];

void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event)
{
    /* initialization */
    size_t rx_size = 0;
    int result = 0;

    /* read */
    esp_err_t ret = tinyusb_cdcacm_read(itf, usb_buf, CONFIG_TINYUSB_CDC_RX_BUFSIZE, &rx_size);
    if (ret == ESP_OK) {
        //ESP_LOGI(TAG, "Data from channel %d:", itf);
        //ESP_LOG_BUFFER_HEXDUMP(TAG, usb_buf, rx_size, ESP_LOG_INFO);
        result = usb_process_rx_buf(usb_buf, &rx_size);
    } else {
        //ESP_LOGE(TAG, "Read error");
    }

    /* write back */
    if (result >= 0) {
        tinyusb_cdcacm_write_queue(itf, usb_buf, rx_size);
        tinyusb_cdcacm_write_flush(itf, 0);
    }
}

//static void init_usb()

static void led_task(void * data)
{
    LedState * state = (LedState *)(data);
    while (1) {
        for (int i_blink = 0; i_blink < 4; i_blink++) {
            if (state->rgb_blink) {
                // Set duty to 10%
                ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, GREEN_LED_CHANNEL, (unsigned int)(round(0.001 * (float)(1 << 10)))));
                // Update duty to apply the new value
                ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, GREEN_LED_CHANNEL));
            }
            vTaskDelay( pdMS_TO_TICKS(100));
            xSemaphoreTake(d_sem_pwm, portMAX_DELAY);
            // Set duty to 10%
            ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, GREEN_LED_CHANNEL, 0));
            // Update duty to apply the new value
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, GREEN_LED_CHANNEL));
            xSemaphoreGive(d_sem_pwm);
            vTaskDelay( pdMS_TO_TICKS(100));
        }
        vTaskDelay( pdMS_TO_TICKS(500));
    }
    vTaskDelete(NULL);
}

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    if (gpio_get_level(CAM_OUT_PIN) == 0) {
       if (d_frame_detected == 1) {
            // if (d_fake_frames == 0) {
            int result = fake_frame();
            if (result == 1) {
                d_fake_frames++;
            } else if (result == 0) {
                d_frame_number++;
            } else {
                d_meas_error = 1;
            }
            if (d_in_meas == 1) { 
                if (ir_set_led4frame() < 0) {
                    d_meas_error = 1;
                }
            }
            d_frame_detected = 0;
       }
    } else {
       d_frame_detected = 1;
       frame_tic();
    }
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    d_reboot_lock = 0;

    d_sem_pwm = xSemaphoreCreateBinary();
    d_in_meas = 0;

    // USB Init
    ESP_LOGI(TAG, "USB initialization");
    const tinyusb_config_t tusb_cfg = {
        .device_descriptor = NULL,
        .string_descriptor = NULL,
        .external_phy = false,
        .configuration_descriptor = NULL,
    };

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    tinyusb_config_cdcacm_t acm_cfg = {
        .usb_dev = TINYUSB_USBDEV_0,
        .cdc_port = TINYUSB_CDC_ACM_0,
        .rx_unread_buf_sz = 64,
        .callback_rx = &tinyusb_cdc_rx_callback, // the first way to register a callback
        .callback_rx_wanted_char = NULL,
        .callback_line_state_changed = NULL,
        .callback_line_coding_changed = NULL
    };

    ESP_ERROR_CHECK(tusb_cdc_acm_init(&acm_cfg));

    gpio_config_t gpio_conf = {
        // config gpios
        .pin_bit_mask = (1ULL << TRIGGER_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    // set the gpios as per gpio_conf
    ESP_ERROR_CHECK(gpio_config(&gpio_conf));

    // gpio_config_t gpio_conf_dpwr = {
    //     // config gpios
    //     .pin_bit_mask = (1ULL << DISPLAY_POWER_PIN),
    //     .mode = GPIO_MODE_OUTPUT,
    //     .pull_up_en = GPIO_PULLUP_DISABLE,
    //     .pull_down_en = GPIO_PULLDOWN_DISABLE,
    //     .intr_type = GPIO_INTR_DISABLE,
    // };
    // // set the gpios as per gpio_conf
    // ESP_ERROR_CHECK(gpio_config(&gpio_conf_dpwr));
    // gpio_set_level((gpio_num_t)DISPLAY_POWER_PIN, 0);

    gpio_config_t gpio_conf_int = {
        // config gpios
        .pin_bit_mask = (1ULL << CAM_OUT_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_ANYEDGE, // GPIO_INTR_NEGEDGE,
    };
    // set the gpios as per gpio_conf
    ESP_ERROR_CHECK(gpio_config(&gpio_conf_int));
    ESP_ERROR_CHECK(gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT));
    ESP_ERROR_CHECK(gpio_isr_handler_add(CAM_OUT_PIN, gpio_isr_handler, NULL));
    d_frame_number = 0;
    d_frame_detected = 1;
    d_fake_frames = 0;

    // Set the LEDC peripheral configuration
    ir_led_init();

    xSemaphoreGive(d_sem_pwm);
    xTaskCreate(led_task, "ledtask", 2048, &d_state, LED_TASK_PRIO, NULL);
    //
    // for (int i_led = 0; i_led < N_LEDS; i_led++) {
    //     ir_set_duty(i_led, 0.1*i_led);
    // }

    // while (1) {;}
}
