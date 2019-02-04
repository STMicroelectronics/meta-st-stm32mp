# Generate boot.scr.uimg:
# ./tools/mkimage -C none -A arm -T script -d boot.src.cmd boot.scr.uimg
#

# M4 Firmware load
env set m4fw_name "rproc-m4-fw.elf"
env set m4fw_addr ${kernel_addr_r}
env set boot_m4fw 'rproc init; rproc load 0 ${m4fw_addr} ${filesize}; rproc load_rsc 0 ${m4fw_addr} ${filesize}; rproc start 0'

# boot M4 Firmware when available
env set scan_m4fw 'if test -e ${devtype} ${devnum}:${distro_bootpart} ${m4fw_name};then echo Found M4 FW $m4fw_name; if load ${devtype} ${devnum}:${distro_bootpart} ${m4fw_addr} ${m4fw_name}; then run boot_m4fw; fi; fi;'

# Update DISTRO command= search in sub-directory and load M4 firmware
env set boot_prefixes "/${boot_device}${boot_instance}_${board_name}_"
env set boot_extlinux "run scan_m4fw;${boot_extlinux}"

if test ${boot_device} = mmc; then
    if test ${distro_bootpart} > 4; then
        env set boot_prefixes "/mmc${boot_instance}_${board_name}-optee_"
    fi

    #start the correct exlinux.conf
    run scan_dev_for_boot_part

elif test ${boot_device} = nand; then

    #start the correct exlinux.conf without remount UBI
    run scan_dev_for_boot

elif test ${boot_device} = nor; then

    #SDCARD boot
    run bootcmd_mmc0

    #NAND boot
    env set boot_prefixes "/nand0_${board_name}_"
    run bootcmd_ubifs0

    #EMMC boot
    env set boot_prefixes "/${boot_device}${boot_instance}-mmc1_${board_name}_"
    run bootcmd_mmc1
fi

echo SCRIPT FAILED... ${boot_prefixes}extlinux/extlinux.conf not found !
