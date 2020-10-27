require tf-a-stm32mp-common.inc

SUMMARY = "Trusted Firmware-A SSP for STM32MP1"
SECTION = "bootloaders"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://license.rst;md5=1dd070c98a281d18d9eefd938729b031"

SRC_URI = "git://github.com/ARM-software/arm-trusted-firmware.git;protocol=https;nobranch=1"
SRCREV = "a04808c16cfc126d9fe572ae7c4b5a3d39de5796"

SRC_URI += " \
    file://0001-st-update-v2.2-r2.0.0.patch \
    \
    file://0100-v2.2-stm32mp-ssp-r2-rc2.patch \
    "

TF_VERSION = "2.2"
PV = "${TF_VERSION}.r2"

S = "${WORKDIR}/git"

PROVIDES += "virtual/trusted-firmware-a-ssp"

TFA_SHARED_SOURCES = "0"

TF_A_BASENAME = "tf-a-ssp"
TF_A_CONFIG = "ssp"
TF_A_CONFIG_ssp = " STM32MP_SSP=1 "

# Configure stm32mp1 make settings
EXTRA_OEMAKE += 'PLAT=stm32mp1'
EXTRA_OEMAKE += 'ARCH=aarch32'
EXTRA_OEMAKE += 'ARM_ARCH_MAJOR=7'
EXTRA_OEMAKE += 'STM32MP_UART_PROGRAMMER=1'
EXTRA_OEMAKE += 'STM32MP_USB_PROGRAMMER=1'

# ---------------------------------
# Configure archiver use
# ---------------------------------
include ${@oe.utils.ifelse(d.getVar('ST_ARCHIVER_ENABLE') == '1', 'tf-a-stm32mp-ssp-archiver.inc','')}

# ---------------------------------
# Configure devupstream class usage
# ---------------------------------
BBCLASSEXTEND = "devupstream:target"

SRC_URI_class-devupstream = "git://github.com/STMicroelectronics/arm-trusted-firmware.git;protocol=https;branch=v${TF_VERSION}-r2-stm32mp-ssp"
SRCREV_class-devupstream = "91745e6389486247c8a4b11cc428f9ce235f319e"

# ---------------------------------
# Configure default preference to manage dynamic selection between tarball and github
# ---------------------------------
STM32MP_SOURCE_SELECTION ?= "tarball"

DEFAULT_PREFERENCE = "${@bb.utils.contains('STM32MP_SOURCE_SELECTION', 'github', '-1', '1', d)}"
