#!/bin/bash

libs="$BASE_PATH/lib/hyundai/x86_64"

if [ "$1" = "hyundai" ]
then
    libs="$BASE_PATH/lib/hyundai/x86_64"
elif [ "$1" = "kia" ]
then
    libs="$BASE_PATH/lib/kia/x86_64"
else
    echo "First argument must be hyundai or kia"
    exit
fi

LD_LIBRARY_PATH=$libs java -Djava.library.path=$libs -jar $BASE_PATH/target/main-1.0.jar $2 $3