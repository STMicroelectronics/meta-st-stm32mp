SUMMARY = "Provide 'extlinux.conf' file for U-Boot"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

DEPENDS += "u-boot-mkimage-native"

PACKAGE_ARCH = "${MACHINE_ARCH}"

SRC_URI = "file://boot.scr.cmd"

PV = "3.1.1"

inherit kernel-arch extlinuxconf-stm32mp

B = "${WORKDIR}/build"

UBOOT_EXTLINUX_BOOTSCR = "${WORKDIR}/boot.scr.cmd"
UBOOT_EXTLINUX_BOOTSCR_IMG = "${B}/boot.scr.uimg"

UBOOT_EXTLINUX_INSTALL_DIR ?= "/boot"

do_compile() {
    # If there is only one configuration, we try to figure out if we can cleanup
    # to have a single exlinux.conf file on extlinux folder (to avoid using boot.scr script).
    if [ "$(find ${B}/* -maxdepth 0 -type d | wc -w)" -eq 1 ] ; then
        subdir=$(find ${B}/* -maxdepth 0 -type d)
        bbnote "Only one subdir found for extlinux.conf files: ${subdir}"
        # If there is the <DEVICETREE>_extlinux.conf file, then rename it to 'extlinux.conf'
        # and use also default subdir name for u-boot (i.e. 'extlinux')
        if [ "$(echo ${STM32MP_DEVICETREE} | wc -w)" -eq 1 ] ; then
            dvtree=$(echo ${STM32MP_DEVICETREE})
            bbnote "Only one devicetree defined: ${dvtree}"
            if [ -f ${subdir}/${dvtree}_extlinux.conf ]; then
                bbnote "Moving ${dvtree}_extlinux.conf to extlinux.conf file"
                mv -f ${subdir}/${dvtree}_extlinux.conf ${subdir}/extlinux.conf
            fi
            if [ "$(basename ${subdir})" != "extlinux" ]; then
                bbnote "Moving $(basename ${subdir}) to extlinux subdir"
                mv -f ${subdir} ${B}/extlinux
            fi
        fi
    fi
    # Generate boot script only when multiple extlinux.conf file are set
    if [ "$(find ${B}/* -name '*extlinux.conf' | wc -w)" -gt 1 ]; then
        mkimage -C none -A ${UBOOT_ARCH} -T script -d ${UBOOT_EXTLINUX_BOOTSCR} ${UBOOT_EXTLINUX_BOOTSCR_IMG}
    fi
}

do_install() {
    install -d ${D}/${UBOOT_EXTLINUX_INSTALL_DIR}
    # Install boot script
    if [ -e ${UBOOT_EXTLINUX_BOOTSCR_IMG} ]; then
        install -m 755 ${UBOOT_EXTLINUX_BOOTSCR_IMG} ${D}/${UBOOT_EXTLINUX_INSTALL_DIR}
    fi
    # Install extlinux files
    if ! [ -z "$(ls -A ${B})" ]; then
        cp -r ${B}/* ${D}/${UBOOT_EXTLINUX_INSTALL_DIR}
    fi
}
FILES_${PN} = "${UBOOT_EXTLINUX_INSTALL_DIR}"
