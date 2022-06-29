require tf-a-stm32mp-common.inc

SUMMARY = "Trusted Firmware-A for STM32MP1"
SECTION = "bootloaders"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://license.rst;md5=1dd070c98a281d18d9eefd938729b031"

PROVIDES += "virtual/trusted-firmware-a"

FILESEXTRAPATHS:prepend:stm32mpcommon := "${THISDIR}/tf-a-stm32mp:"

SRC_URI = "git://github.com/ARM-software/arm-trusted-firmware.git;protocol=https;nobranch=1"
SRCREV = "a1f02f4f3daae7e21ee58b4c93ec3e46b8f28d15"

SRC_URI += " \
     file://0001-v2.6-stm32mp-r1.patch \
     "

TF_A_VERSION = "v2.6"
TF_A_SUBVERSION = "stm32mp"
TF_A_RELEASE = "r1"
PV = "${TF_A_VERSION}-${TF_A_SUBVERSION}-${TF_A_RELEASE}"

ARCHIVER_ST_BRANCH = "${TF_A_VERSION}-${TF_A_SUBVERSION}"
ARCHIVER_ST_REVISION = "${PV}"
ARCHIVER_COMMUNITY_BRANCH = "master"
ARCHIVER_COMMUNITY_REVISION = "${TF_A_VERSION}"

S = "${WORKDIR}/git"

# Configure settings
TFA_PLATFORM  = "stm32mp1"
TFA_ARM_MAJOR = "7"
TFA_ARM_ARCH  = "aarch32"

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

SRC_URI:class-devupstream = "git://github.com/STMicroelectronics/arm-trusted-firmware.git;protocol=https;branch=${ARCHIVER_ST_BRANCH}"
SRCREV:class-devupstream = "c6da17964e4260944af2a703171a3c36b9e3edf8"

# ---------------------------------
# Configure default preference to manage dynamic selection between tarball and github
# ---------------------------------
STM32MP_SOURCE_SELECTION ?= "tarball"

DEFAULT_PREFERENCE = "${@bb.utils.contains('STM32MP_SOURCE_SELECTION', 'github', '-1', '1', d)}"
