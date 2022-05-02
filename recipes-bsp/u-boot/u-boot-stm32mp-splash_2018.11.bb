SUMMARY = "Universal Boot Loader Splash Screen for stm32mp embedded devices"
#TODO Need to review the exact license we want to have for the specific BMP we provide.
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://LICENSE;md5=ac3e0fd89b582e9fc11d534a27636636"

SRC_URI = "${@bb.utils.contains('MACHINE_FEATURES', 'splashscreen', 'file://${UBOOT_SPLASH_LANDSCAPE_SRC} file://${UBOOT_SPLASH_PORTRAIT_SRC}', '', d)}"
SRC_URI += "file://LICENSE"

S = "${WORKDIR}"

UBOOT_SPLASH_LANDSCAPE_SRC = "ST_logo_2020_blue_H_rgb_rle8_480x183.bmp"
UBOOT_SPLASH_PORTRAIT_SRC = "ST_logo_2020_blue_H_rgb_rle8_183x480.bmp"
UBOOT_SPLASH_LANDSCAPE_IMAGE ?= "splash_landscape"
UBOOT_SPLASH_PORTRAIT_IMAGE ?= "splash_portrait"

inherit deploy

do_compile[noexec] = "1"

do_install() {
    install -d ${D}/boot
    if [ -e "${S}/${UBOOT_SPLASH_LANDSCAPE_SRC}" ]; then
        install -m 644 ${S}/${UBOOT_SPLASH_LANDSCAPE_SRC} ${D}/boot/${UBOOT_SPLASH_LANDSCAPE_IMAGE}.bmp
    fi
    if [ -e "${S}/${UBOOT_SPLASH_PORTRAIT_SRC}" ]; then
        install -m 644 ${S}/${UBOOT_SPLASH_PORTRAIT_SRC} ${D}/boot/${UBOOT_SPLASH_PORTRAIT_IMAGE}.bmp
    fi
}

ALLOW_EMPTY:${PN} = "1"
FILES:${PN} = "/boot"
