#!/bin/sh

COMPATIBLE_BOARD=$(cat /proc/device-tree/compatible | sed "s|st,|,|g" | cut -d ',' -f2)

case $COMPATIBLE_BOARD in
stm32mp151a-dk2*)
	# supported
	;;
stm32mp151f-dk2*)
	# supported
	;;
stm32mp153a-dk2*)
	# supported
	;;
stm32mp153f-dk2*)
	# supported
	;;
stm32mp157c-dk2*)
	# supported
	;;
stm32mp157f-dk2*)
	# supported
	;;
	*)
	exit 1;
esac

case $1 in
bind)
	echo "bind driver hci_uart";
	echo "bind driver hci_uart" > /dev/kmsg
	modprobe -r hci_uart
	modprobe hci_uart
	;;
unbind)
	echo "unbind driver hci_uart";
	echo "unbind driver hci_uart" > /dev/kmsg
	modprobe -r hci_uart
	;;
*)
	echo "$0 [bind|unbind]"
	echo "Bind/Unbind bluetooth driver brcmfmac."
	;;
esac
