#!/bin/sh

case $1 in
start)
    echo enabled >/sys/class/remoteproc/remoteproc0/coredump
    ;;
stop)
    echo disabled >/sys/class/remoteproc/remoteproc0/coredump
    ;;

*)
    echo "$0 [start|stop]"
    echo "Start/Stop M4 Fwirmware coredump."
    ;;
esac

