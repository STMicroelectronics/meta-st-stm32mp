#!/bin/sh
case "$1" in
    start)
        echo 1 > /sys/devices/system/cpu/cpufreq/boost
        ;;
    stop)
        echo 0 > /sys/devices/system/cpu/cpufreq/boost
        ;;
esac
exit 0
