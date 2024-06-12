#!/bin/sh

rproc_class_dir="/sys/class/remoteproc/remoteproc0/"
fmw_dir="/lib/firmware"
project_name=$(basename $(pwd))

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

if [ -z "${arg_t}" ]; then
    # By default the firmware is non secure
    fw_type="CM33_NonSecure"
else
    case "$arg_t" in

        ns)
            fw_type="CM33_NonSecure"
            ;;

        s)
            fw_type="CM33_Secure"
            ;;

        ns_s)
            fw_type="CM33_NonSecure_*"
            ;;

        *)
            printf "\n\033[1;31m Error:\033[0m Invalid option: \033[1;31m-t $arg_t\033[0m\n\n"
            usage
            exit 1
            ;;
    esac
fi
action=$1

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
        if [ ${fw_type} != "CM33_NonSecure" ]; then
        echo  "Error: only non secure firmware supported"
        exit 1
        fi
        fmw_name="${fmw_basename}.elf"
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
        echo "Nothing to do, no Cortex-M fw is running"
    else
        echo stop > $rproc_class_dir/state
    fi
fi
