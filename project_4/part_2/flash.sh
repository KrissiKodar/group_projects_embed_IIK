#!/bin/bash
#  Short script to toggle a GPIO pin at the highest frequency
#  possible using Bash - by Derek Molloy
echo 17 > /sys/class/gpio/export

# encoder input
echo 19 > /sys/class/gpio/export
echo 26 > /sys/class/gpio/export


sleep 0.5
echo "out" > /sys/class/gpio/gpio17/direction
echo "in" > /sys/class/gpio/gpio19/direction
echo "in" > /sys/class/gpio/gpio26/direction

COUNTER=0
while [ $COUNTER -lt 100 ]; do
    echo 1 > /sys/class/gpio/gpio17/value
    let COUNTER=COUNTER+1
    sleep 0.1
    echo 0 > /sys/class/gpio/gpio17/value
    sleep 0.1
done

echo 17 > /sys/class/gpio/unexport
echo 19 > /sys/class/gpio/unexport
echo 26 > /sys/class/gpio/unexport