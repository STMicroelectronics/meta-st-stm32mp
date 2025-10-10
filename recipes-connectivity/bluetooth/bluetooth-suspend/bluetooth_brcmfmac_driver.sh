#!/bin/sh

COMPATIBLE_BOARD=$(tr -d '\0' < /proc/device-tree/compatible | sed "s|st,|,|g" | cut -d ',' -f2 | head -n 1 |tr '\n' ' ' | sed "s/ //g")

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

stm32mp257f-dk*)
	# supported
	;;

	*)
	exit 1;
esac

case $1 in
"bind")
	echo "bind driver hci_uart";
	echo "bind driver hci_uart" > /dev/kmsg
	if [ 0 -ne $(lsmod | grep -c 'hci_uart') ]; then
		modprobe -r hci_uart
	fi
	modprobe hci_uart
	;;
"unbind")
	if [ 0 -ne $(lsmod | grep -c 'hci_uart') ]; then
		echo "unbind driver hci_uart";
		echo "unbind driver hci_uart" > /dev/kmsg
		modprobe -r hci_uart
	fi
	;;
*)
	echo "$0 [bind|unbind]"
	echo "Bind/Unbind bluetooth driver brcmfmac."
	;;
esac
