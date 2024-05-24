# Version
cmake_minimum_required(VERSION 3.10)

# Name
set(PROJECT_NAME camtest)
project(${PROJECT_NAME})

# Main paths
set(MAIN_PATH "../../")

# Apply settings
include(${BUILD_FOLDER}/settings.cmake)

# Flags
set(MAIN_FLAGS "-Wall -Ofast -pthread -std=c++17 -lrt -lm -lESEUsbCam")
set(ALL_CXX_FLAGS "${MAIN_FLAGS} ${PLATFORM_FLAGS}")
set(ALL_C_FLAGS "-Wall -std=c11 -Ofast -lrt -pthread -lm -lESEUsbCam")
list(APPEND ALL_C_FLAGS ${PLATFORM_C_FLAGS})

# Include 
set(ALL_INCLUDE_PATH "")
# Platform dependent includes
list(APPEND ALL_INCLUDE_PATH ${PLATFORM_PATH})

# standards & options
SET(CMAKE_CXX_FLAGS "${ALL_CXX_FLAGS}")
SET(CMAKE_C_FLAGS "${ALL_C_FLAGS}")

# main cpp
set(MAIN_SOURCE "camtest.cpp")

# set source
set(SOURCE_LIST ${MAIN_SOURCE})

# include other
include(${MAIN_PATH}/eseircam/tools/esecamtools.cmake)
include(${MAIN_PATH}/ipc/ipc_comm.cmake)

# add sources
#message(STATUS "${PROJECT_NAME} sources = ${SOURCE_LIST}")
add_executable(${PROJECT_NAME} ${SOURCE_LIST})

# include & link
target_include_directories(${PROJECT_NAME} PRIVATE ${ALL_INCLUDE_PATH} ${ALL_LIBS_PATH})
target_link_libraries(${PROJECT_NAME} PRIVATE ${ALL_LIBS})
target_link_libraries(${PROJECT_NAME} PRIVATE rt)
target_link_libraries(${PROJECT_NAME} PRIVATE ESEUsbCam)
