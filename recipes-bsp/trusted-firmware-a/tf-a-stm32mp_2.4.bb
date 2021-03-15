require tf-a-stm32mp-common.inc

SUMMARY = "Trusted Firmware-A for STM32MP1"
SECTION = "bootloaders"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://license.rst;md5=1dd070c98a281d18d9eefd938729b031"

PROVIDES += "virtual/trusted-firmware-a"

FILESEXTRAPATHS_prepend_stm32mpcommon := "${THISDIR}/tf-a-stm32mp:"

SRC_URI = "git://github.com/ARM-software/arm-trusted-firmware.git;protocol=https;nobranch=1"
SRCREV = "e2c509a39c6cc4dda8734e6509cdbe6e3603cdfc"

SRC_URI += " \
    file://0001-st-update-v2.4-r1.0.0.patch \
    "

TF_VERSION = "2.4"
PV = "${TF_VERSION}.r1"

S = "${WORKDIR}/git"

# Configure stm32mp1 make settings
EXTRA_OEMAKE += "PLAT=stm32mp1"
EXTRA_OEMAKE += "ARCH=aarch32"
EXTRA_OEMAKE += "ARM_ARCH_MAJOR=7"

# Enable the wrapper for debug
TF_A_ENABLE_DEBUG_WRAPPER ?= "1"

# ---------------------------------
# Configure archiver use
# ---------------------------------
include ${@oe.utils.ifelse(d.getVar('ST_ARCHIVER_ENABLE') == '1', 'tf-a-stm32mp-archiver.inc','')}

# ---------------------------------
# Configure devupstream class usage
# ---------------------------------
BBCLASSEXTEND = "devupstream:target"

SRC_URI_class-devupstream = "git://github.com/STMicroelectronics/arm-trusted-firmware.git;protocol=https;branch=v${TF_VERSION}-stm32mp"
SRCREV_class-devupstream = "40572e4067f205691a4dca3b5fdabe3a40c69594"

# ---------------------------------
# Configure default preference to manage dynamic selection between tarball and github
# ---------------------------------
STM32MP_SOURCE_SELECTION ?= "tarball"

DEFAULT_PREFERENCE = "${@bb.utils.contains('STM32MP_SOURCE_SELECTION', 'github', '-1', '1', d)}"
