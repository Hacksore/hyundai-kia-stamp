#!/bin/bash

echo "Running in env: $(uname -m)"
libs="$BASE_PATH/lib/hyundai/arm64-v8a"

if [ "$1" = "hyundai" ]
then
    libs="$BASE_PATH/lib/hyundai/arm64-v8a"
elif [ "$1" = "kia" ]
then
    libs="$BASE_PATH/lib/kia/arm64-v8a"
else
    echo "First argument must be hyundai or kia"
    exit
fi

LD_LIBRARY_PATH=$libs java -Djava.library.path=$libs -jar $BASE_PATH/target/main-1.0.jar $2 $3