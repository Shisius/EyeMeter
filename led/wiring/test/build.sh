#!/usr/bin/bash

g++ gpio_test.cpp -Wall -pthread -Ofast -lwiringPi -lwiringPiDev -o gpio_test
