SUMMARY = "Trusted Firmware-A for STM32MP1"
SECTION = "bootloaders"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://license.rst;md5=e927e02bca647e14efd87e9e914b2443"

SRC_URI = "https://github.com/ARM-software/arm-trusted-firmware/archive/v${PV}.tar.gz"
SRC_URI[md5sum] = "21038abbf572c273fa87d296bcd5dad2"
SRC_URI[sha256sum] = "7d699a1683bb7a5909de37b6eb91b6e38db32cd6fc5ae48a08eb0718d6504ae4"

SRC_URI += " \
    file://0001-st-update-r1.patch \
    file://0002-st-update-r1.1.0.patch \
    "

TF_VERSION = "2.0"
PV = "${TF_VERSION}"

S = "${WORKDIR}/arm-trusted-firmware-${PV}"

require tf-a-stm32mp-common.inc

# ---------------------------------
# Configure archiver use
# ---------------------------------
include ${@oe.utils.ifelse(d.getVar('ST_ARCHIVER_ENABLE') == '1', 'tf-a-stm32mp-archiver.inc','')}

# ---------------------------------
# Configure devupstream class usage
# ---------------------------------
BBCLASSEXTEND = "devupstream:target"

SRC_URI_class-devupstream = "git://github.com/STMicroelectronics/arm-trusted-firmware.git;protocol=https;name=tfa;branch=v2.0-stm32mp"
SRCREV_class-devupstream = "d0233623681124a85b069f97a447d7edb1cc1c02"
SRCREV_FORMAT_class-devupstream = "tfa"
PV_class-devupstream = "${TF_VERSION}+github+${SRCPV}"
# ---------------------------------
# Configure default preference to manage dynamic selection between tarball and github
# ---------------------------------
STM32MP_SOURCE_SELECTION ?= "tarball"

DEFAULT_PREFERENCE = "${@bb.utils.contains('STM32MP_SOURCE_SELECTION', 'github', '-1', '1', d)}"
