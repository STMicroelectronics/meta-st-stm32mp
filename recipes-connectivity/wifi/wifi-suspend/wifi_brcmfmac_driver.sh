#!/bin/sh
SDMMC_PATH=/sys/bus/amba/drivers/mmci-pl18x

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

if [ -d $SDMMC_PATH ];
then

	if [ ! -d /sys/bus/sdio/drivers/brcmfmac ];
	then
		exit 1;
	fi

case $1 in
bind)
	echo "bind driver brcmfmac/sdmmc";
	echo "bind driver brcmfmac/sdmmc" > /dev/kmsg
	echo 58007000.sdmmc >  $SDMMC_PATH/bind
	;;
unbind)
	echo "unbind driver brcmfmac/sdmmc";
	echo "unbind driver brcmfmac/sdmmc" > /dev/kmsg
	echo 58007000.sdmmc >  $SDMMC_PATH/unbind
	;;
*)
	echo "$0 [bind|unbind]"
	echo "Bind/Unbind wifi driver brcmfmac."
	;;
esac

fi
