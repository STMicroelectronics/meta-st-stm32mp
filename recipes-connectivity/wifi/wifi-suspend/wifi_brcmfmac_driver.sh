#!/bin/sh
SDMMC_PATH=/sys/bus/amba/drivers/mmci-pl18x

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
