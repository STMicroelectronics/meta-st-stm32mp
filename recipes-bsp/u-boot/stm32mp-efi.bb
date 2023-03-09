SUMMARY = "Provide 'ubootefi.var' file for U-Boot efi boot"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

DEPENDS += "u-boot-mkimage-native"

PACKAGE_ARCH = "${MACHINE_ARCH}"

SRC_URI = " \
    file://boot.scr.cmd \
    file://ubootefi.var \
"

inherit kernel-arch

UBOOT_EXTLINUX_BOOTSCR = "${WORKDIR}/boot.scr.cmd"
UBOOT_EXTLINUX_BOOTSCR_IMG = "${WORKDIR}/boot.scr.uimg"

PV = "5.0.0"

UBOOT_EFI_INSTALL_DIR ?= "/boot"

do_compile() {
    mkimage -C none -A ${UBOOT_ARCH} -T script -d ${UBOOT_EXTLINUX_BOOTSCR} ${UBOOT_EXTLINUX_BOOTSCR_IMG}
}
do_install() {
    install -d ${D}/${UBOOT_EFI_INSTALL_DIR}
    install -m 0644 ${WORKDIR}/ubootefi.var ${D}/${UBOOT_EFI_INSTALL_DIR}/
    install -m 0644 ${UBOOT_EXTLINUX_BOOTSCR_IMG} ${D}/${UBOOT_EFI_INSTALL_DIR}/
}
FILES:${PN} = "${UBOOT_EFI_INSTALL_DIR}"
