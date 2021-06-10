#!/bin/bash

echo "Running in env: $(uname -m)"
libs="$BASE_PATH/lib/hyundai/arm"

if [ "$1" = "hyundai" ]
then
    libs="$BASE_PATH/lib/hyundai/arm"
elif [ "$1" = "kia" ]
then
    libs="$BASE_PATH/lib/kia/arm"
else
    echo "First argument must be hyundai or kia"
    exit
fi

LD_LIBRARY_PATH=$libs java -Djava.library.path=$libs -jar $BASE_PATH/target/main-1.0.jar $2 $3