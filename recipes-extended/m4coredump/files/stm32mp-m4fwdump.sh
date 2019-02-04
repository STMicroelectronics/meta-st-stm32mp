#!/bin/sh

mkdir -p /var/crash
timestamp=$(date +%F_%H-%M-%S)
filename=/var/crash/m4-fw-error_${timestamp}.dump
cat /sys/${DEVPATH}/data > ${filename}
echo 1 > /sys/${DEVPATH}/data
