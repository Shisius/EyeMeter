cmake_minimum_required(VERSION 3.16)

add_compile_options(-w) # Turn off warnings until esp_tinyusb is updated IEC-86
set(COMPONENTS main)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(ir_led_ctl)
