include("${MAIN_PATH}/eseircam/tools/esecamtools.cmake")
include("${MAIN_PATH}/led/serial/serial_comm.cmake")
include("${MAIN_PATH}/led/wiring/gpio_tools.cmake")

list(APPEND ALL_INCLUDE_PATH "${MAIN_PATH}/eseircam/master")

list(APPEND SOURCE_LIST "${MAIN_PATH}/eseircam/master/esecam_master.cpp")
