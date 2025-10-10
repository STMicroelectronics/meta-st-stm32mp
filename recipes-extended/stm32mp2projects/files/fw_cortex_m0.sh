#!/bin/sh

rproc_class_dir="/sys/class/remoteproc/remoteproc1/"
fmw_dir="/lib/firmware"
project_name=$(basename $(pwd))
# By default the firmware is non secure
fw_type="NonSecure"

TARGET_REMOTEPROC_NAME="m0"
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
        echo "[ERROR] no sysfs entry for m0 found on /sys/class/remoteproc/"
        exit 1
    fi
}

usage()
{
   # Display Help
   echo "start stop the remote processor firmware."
   echo
   echo "Syntax: ${0} [-t <ns|s|ns_s>] <start|stop>"
   echo " -t:"
   echo "   ns   Load a non secure firmware (Default)."
   echo "   s    Load a non secure firmware."
   echo "   ns_s Load a TF-M + non secure firmwares."
   echo
   echo " start: Start the firmware."
   echo " stop:  Stop the firmware."
   echo
}

while getopts ":t:" o; do
    case "${o}" in
        t)
            arg_t=${OPTARG}
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

action=$1

get_remoteproc_sysfs_entry

case $action in
    start) ;;

    stop) ;;

    *) echo "`basename ${0}`:usage: start | stop"
        printf "\n\033[1;31m Error:\033[0m Invalid Action: \033[1;31m-t $action\033[0m\n\n"
        exit 1
        ;;
esac

rproc_state=`tr -d '\0' < $rproc_class_dir/state`

#################
# Start example #
#################
if [ $action == "start" ]; then
    fmw_basename="${project_name}_${fw_type}"

    if [ `cat ${rproc_class_dir}/fw_format` = "TEE" ]; then
        #The firmware is managed by OP-TEE, it must be signed.
        # get the name based depending on firmware present and -t option 
        fmw_name="`ls lib/firmware/${fmw_basename}_sign.bin`"
        if [ -z "${fmw_name}" ]; then
           echo  "Error: signed firmware ${fmw_basename}_sign.bin cannot be found"
           exit 1
        fi
        fmw_name="`basename ${fmw_name}`"
    else
        #The firmware is managed by Linux, it must be an ELF.
        fmw_name="${fmw_basename}.elf"
        if [ -e lib/firmware/${fmw_basename}_stripped.elf ]; then
            fmw_name="${fmw_basename}_stripped.elf"
        fi
    fi

    if [ ! -e lib/firmware/${fmw_name} ]; then
          echo  "Error: signed firmware ${fmw_name} cannot be found"
          exit 1
    fi

    echo "`basename ${0}`: fmw_name=${fmw_name}"

    if [ $rproc_state == "running" ]; then
        echo "Stopping running fw ..."
        echo stop > $rproc_class_dir/state
    fi

    # Create /lib/firmware directory if not exist
    if [ ! -d $fmw_dir ]; then
        echo "Create $fmw_dir directory"
        mkdir $fmw_dir
    fi

    # Copy firmware in /lib/firmware
    cp lib/firmware/$fmw_name $fmw_dir/

    # load and start firmware
    echo $fmw_name > $rproc_class_dir/firmware
    echo start > $rproc_class_dir/state
fi

################
# Stop example #
################
if [ $action == "stop" ]; then

    if [ $rproc_state == "offline" ]; then
        echo "Nothing to do, no Cortex-M0 fw is running"
    else
        echo stop > $rproc_class_dir/state
    fi
fi
