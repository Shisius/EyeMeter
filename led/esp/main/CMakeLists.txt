idf_component_register(SRCS "ir_led_ctl.c"
                    INCLUDE_DIRS "."
                    INCLUDE_DIRS "../../../types" 
                    REQUIRES esp_driver_ledc nvs_flash)
                    # INCLUDE_DIRS "~/Tools/LIBS/BMI160_SensorAPI"
