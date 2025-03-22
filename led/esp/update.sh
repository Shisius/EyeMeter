#!/bin/bash

source ~/Downloads/esp-idf/export.sh

result=0
idf.py dfu && result=1

if [ $result == 1 ]
then
	idf.py dfu-flash && result=2
else
	echo "Firmware build failed"
fi

if [ $result == 2 ]
then
	echo "Success"
else
	echo "Firmware flash failed"
fi
