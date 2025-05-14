#!/bin/bash
# tmp dir
if [ ! -d "tmp" ]
then
	echo "Creating tmp dir"
	mkdir tmp
fi
cd tmp/
# run cmake
result=0
echo "Running cmake"
cmake -DBUILD_FOLDER="${PWD}/.." -DDEFINE_90HZ_MODE=OFF -DDEFINE_NOESP_MODE=ON ../../ 
echo "Building project"
make -j12 && result=1
if [ $result == 1 ]
then
	echo "Project built successfully"
else
	echo "Building failed"
fi

