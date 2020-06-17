#!/bin/sh -

autodetect_board() {
if [ ! -d /proc/device-tree/ ];
then
    echo "Proc Device tree are not available, Could not detect on which board we are" > /dev/kmsg
    exit 1
fi

#search on device tree compatible entry the board type
if $(grep -q "stm32mp157[acdf]-ev" /proc/device-tree/compatible) ;
then
    board="STM32MP15_M4_EVAL"
else if $(grep -q "stm32mp157[acdf]-dk" /proc/device-tree/compatible) ;
then
    board="STM32MP15_M4_DISCO"
else
    echo "Board is not an EVAL or a DISCO BOARD" > /dev/kmsg
    exit 1
fi
fi
}

copy_default_M4_fw() {
#Test if ${board}_@default_fw@.elf is existing
if [ -z "$(find @userfs_mount_point@/examples/* -name ${board}_@default_fw@.elf)" ]; then
    echo "The default copro example ${board}_@default_fw@ doesn't exist" > /dev/kmsg
    exit 1
else
    #copy ${board}_@default_fw@.elf into /lib/firmware/
    cp $(find @userfs_mount_point@/examples/* -name ${board}_@default_fw@.elf) /lib/firmware/.
fi
}

firmware_load_start() {
# Change the name of the firmware
echo -n ${board}_@default_fw@.elf > /sys/class/remoteproc/remoteproc0/firmware

# Change path to found firmware
#echo -n /home/root >/sys/module/firmware_class/parameters/path

# Restart firmware
echo start >/sys/class/remoteproc/remoteproc0/state

echo "Booting fw image ${board}_@default_fw@.elf" > /dev/kmsg
}

firmware_load_stop() {
# Stop the firmware
if [ $(cat /sys/class/remoteproc/remoteproc0/state) == "running" ]; then
    echo stop >/sys/class/remoteproc/remoteproc0/state
    echo "Stopping fw image ${board}_@default_fw@.elf" > /dev/kmsg
else
    echo "Default copro already stopped" > /dev/kmsg
fi
}

board=""
autodetect_board

case "$1" in
start)
    copy_default_M4_fw
    firmware_load_stop
    firmware_load_start
    ;;
stop)
    firmware_load_stop
    ;;
restart)
    firmware_load_stop
    firmware_load_start
    ;;
*)
    echo "HELP: $0 [start|stop|restart]"
    ;;
esac

exit 0
