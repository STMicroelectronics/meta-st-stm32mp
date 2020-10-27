FILESEXTRAPATHS_prepend_stm32mpcommon := "${THISDIR}/tf-a-stm32mp:"

SECTION = "bootloaders"

LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://license.rst;md5=1dd070c98a281d18d9eefd938729b031"

SRC_URI = "git://github.com/ARM-software/arm-trusted-firmware.git;protocol=https;nobranch=1"
SRCREV = "a04808c16cfc126d9fe572ae7c4b5a3d39de5796"

SRC_URI += " \
    file://0001-st-update-v2.2-r2.0.0.patch \
    file://0002-st-update-v2.2-r2.1.0.patch \
    "

TF_VERSION = "2.2"
PV = "${TF_VERSION}.r2"

S = "${WORKDIR}/git"
