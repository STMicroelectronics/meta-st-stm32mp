SUMMARY = "Provide 'extlinux.conf' file for U-Boot"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

DEPENDS += "u-boot-mkimage-native"

PACKAGE_ARCH = "${MACHINE_ARCH}"

SRC_URI = "file://boot.scr.cmd"

PV = "2.0"

inherit kernel-arch extlinuxconf-stm32mp

B = "${WORKDIR}/build"

UBOOT_EXTLINUX_BOOTSCR = "${WORKDIR}/boot.scr.cmd"
UBOOT_EXTLINUX_BOOTSCR_IMG = "${B}/boot.scr.uimg"

UBOOT_EXTLINUX_INSTALL_DIR ?= "/boot"

do_compile() {
    # Generate boot script only when multiple extlinux subdirs are set
    if [ "$(find ${B}/* -maxdepth 0 -type d | wc -w)" -gt 1 ]; then
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
