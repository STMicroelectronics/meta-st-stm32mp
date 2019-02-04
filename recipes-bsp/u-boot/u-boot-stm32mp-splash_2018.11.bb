SUMMARY = "Universal Boot Loader Splash Screen for stm32mp embedded devices"
#TODO Need to review the exact license we want to have for the specific BMP we provide.
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://LICENSE;md5=ac3e0fd89b582e9fc11d534a27636636"

SRC_URI = "${@bb.utils.contains('MACHINE_FEATURES', 'splashscreen', 'file://${UBOOT_SPLASH_SRC}', '', d)}"
SRC_URI += "${@bb.utils.contains('MACHINE_FEATURES', 'splashscreen', 'file://LICENSE', '', d)}"

S = "${WORKDIR}"

UBOOT_SPLASH_SRC = "stmicroelectronics.bmp"
UBOOT_SPLASH_IMAGE ?= "splash"

inherit deploy

do_compile[noexec] = "1"

do_install() {
    install -d ${D}/boot
    if [ -e "${S}/${UBOOT_SPLASH_SRC}" ]; then
        install -m 644 ${S}/${UBOOT_SPLASH_SRC} ${D}/boot/${UBOOT_SPLASH_IMAGE}.bmp
    fi
}

ALLOW_EMPTY_${PN} = "1"
FILES_${PN} = "/boot"
