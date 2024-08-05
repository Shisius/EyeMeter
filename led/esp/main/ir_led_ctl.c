
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
        ESP_LOGE(TAG, "Read error");
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
        for (int i_blink = 0; i_blink < 3; i_blink++) {
            if (state->rgb_blink) {
                gpio_set_level((gpio_num_t)GREEN_LED_PIN, 1);
            }
            vTaskDelay( pdMS_TO_TICKS(100));
            gpio_set_level((gpio_num_t)GREEN_LED_PIN, 1);
            vTaskDelay( pdMS_TO_TICKS(100));
        }
        vTaskDelay( pdMS_TO_TICKS(1000));
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    // esp_err_t ret = nvs_flash_init();
    // if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    //     ESP_ERROR_CHECK(nvs_flash_erase());
    //     ret = nvs_flash_init();
    // }
    // ESP_ERROR_CHECK( ret );

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

    gpio_config_t gpio_conf_green = {
        // config gpios
        .pin_bit_mask = (1ULL << GREEN_LED_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    // set the gpios as per gpio_conf
    ESP_ERROR_CHECK(gpio_config(&gpio_conf_green));

    // Set the LEDC peripheral configuration
    ir_led_init();

    xTaskCreate(led_task, "ledtask", 2048, &d_state, configMAX_PRIORITIES - 1, NULL);
    //
    // for (int i_led = 0; i_led < N_LEDS; i_led++) {
    //     ir_set_duty(i_led, 0.1*i_led);
    // }

    // while (1) {;}
}
