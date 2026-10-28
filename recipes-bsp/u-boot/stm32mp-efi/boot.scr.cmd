# efi boot management
if test -e ${devtype} ${devnum}:${distro_bootpart} /EFI/BOOT/bootarm.efi; then env set efi_firmware_name '/efi/boot/bootarm.efi'; fi
if test -e ${devtype} ${devnum}:${distro_bootpart} /EFI/BOOT/bootaa64.efi; then env set efi_firmware_name '/efi/boot/bootaa64.efi'; fi

efidebug boot add -b 0000 'kernel' ${devtype} ${devnum}:${distro_bootpart} ${efi_firmware_name} -D ${devtype} ${devnum}:${distro_bootpart} dtb/${fdtfile} -i ${devtype} ${devnum}:${distro_bootpart} st-image-resize-initrd -s "root=PARTUUID=e91c4e10-16e6-4c0e-bd0e-77becf4a3582 rootwait rw console=ttySTM0,115200"
efidebug boot order 0000

run scan_dev_for_efi
