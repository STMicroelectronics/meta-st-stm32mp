#!/bin/sh -

autodetect_board() {
    if [ ! -d /proc/device-tree/ ];
    then
        echo "Proc Device tree are not available, Could not detect on which board we are" > /dev/kmsg
        exit 1
    fi

    LIST="##BOARDS_LIST##"
    board=""
    for b in $LIST; do
        board_lower=$(echo $b | tr '[:upper:]' '[:lower:]')
        comp_board=$(tr -d '\0' < /proc/device-tree/compatible | sed "s|st,|,|g" | cut -d ',' -f2 | head -n 1 |tr '\n' ' ' | sed "s/ //g")
        if [  "$comp_board" =  "$board_lower" ];
        then
            echo "Board $board_lower is compatible"
            board=$b
            break
        fi
    done
    if [ -z "$board" ]; then
        echo "Board is not a valid BOARD (stm32mp257f-dk, stm32mp257f-ev1)" > /dev/kmsg
        exit 0
    fi
}

find_default_project() {
    DEFAULT_PROJECT=""
    if [ -n "$board" ]; then
        if [ -z "$(find @userfs_mount_point@ -name default.$board)" ]; then
            echo "The default copro example for ${board} doesn't exist" > /dev/kmsg
            exit 1
        else
            default_path_project=$(find @userfs_mount_point@/ -name default.$board | head -n 1)
            DEFAULT_PROJECT=$(dirname $default_path_project)
        fi
    fi
}

firmware_load_start() {
    if [ -n "$DEFAULT_PROJECT" ]; then
        cd $DEFAULT_PROJECT
        ./fw_cortex_m33.sh start
        echo "Booting fw image for ${board}" > /dev/kmsg
    fi
}

ucsi_stop_workaround() {
    if [ -e /sys/bus/i2c/drivers/ucsi-stm32g0-i2c/ ]; then
        if [ -e /sys/bus/i2c/drivers/ucsi-stm32g0-i2c/1-0035 ]; then
            echo 1-0035 > /sys/bus/i2c/drivers/ucsi-stm32g0-i2c/unbind
        fi
    fi
}


rproc_class_dir="/sys/class/remoteproc/remoteproc0/"
TARGET_REMOTEPROC_NAME="m33"
REMOTEPROC_DIR="/sys/class/remoteproc"

get_remoteproc_sysfs_entry() {
    rproc_class_dir=""
    for device in "$REMOTEPROC_DIR"/remoteproc*; do
        # Extract the device number
        device_number=$(basename "$device" | sed 's/remoteproc//')

        # Check if the name matches the target name
        if [ -f "$REMOTEPROC_DIR/remoteproc$device_number/name" ]; then
            name=$(cat "$REMOTEPROC_DIR/remoteproc$device_number/name")
            if [ "$name" == "$TARGET_REMOTEPROC_NAME" ]; then
                echo "Found matching remoteproc device: remoteproc$device_number"
                rproc_class_dir="/sys/class/remoteproc/remoteproc$device_number/"
                break
            fi
        fi
    done
    if [ -z "$rproc_class_dir" ]; then
        echo "[ERROR] no sysfs entry for m33 found on /sys/class/remoteproc/"
        exit 1
    fi
}

firmware_load_stop() {
    get_remoteproc_sysfs_entry
    # Stop the firmware
    if [ "$(cat $rproc_class_dir/state)" = "running" ]; then
        if [ -n "$DEFAULT_PROJECT" ]; then
            cd $DEFAULT_PROJECT
            ucsi_stop_workaround
            ./fw_cortex_m33.sh stop
        fi
        echo "Stopping fw image ${board}" > /dev/kmsg
    else
        echo "Default copro already stopped" > /dev/kmsg
    fi
}

board=""
autodetect_board
find_default_project

case "$1" in
start)
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
