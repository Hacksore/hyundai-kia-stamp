#!/bin/sh
DATA_PATH=${BASE_PATH:-.}

libPath="${DATA_PATH}/lib/hyundai/arm/libnative-lib.so"
iv='iv.ccsp.stamp.eu'

if [ "$1" = "hyundai" ]
then
    libPath="$DATA_PATH/lib/hyundai/arm/libnative-lib.so"
elif [ "$1" = "kia" ]
then
    libPath="$DATA_PATH/lib/kia/arm/libnative-lib.so"
else
    echo "First argument must be hyundai or kia"
    exit
fi
./aes_whitebox $libPath $iv $2 $3