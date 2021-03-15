# Adaptation from u-boot-common_${PV}.inc

HOMEPAGE = "http://www.denx.de/wiki/U-Boot/WebHome"
SECTION = "bootloaders"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=5a7450c57ffe5ae63fd732446b988025"

DEPENDS += "dtc-native bc-native"
DEPENDS += "flex-native bison-native"

COMPATIBLE_MACHINE = "(stm32mpcommon)"

SRC_URI = "git://github.com/u-boot/u-boot.git;protocol=https"
SRCREV  = "050acee119b3757fee3bd128f55d720fdd9bb890"

SRC_URI += "\
    file://0001-ARM-v2020.10-stm32mp-r1-MACHINE.patch \
    file://0002-ARM-v2020.10-stm32mp-r1-BOARD.patch \
    file://0003-ARM-v2020.10-stm32mp-r1-MISC-DRIVERS.patch \
    file://0004-ARM-v2020.10-stm32mp-r1-DEVICETREE.patch \
    file://0005-ARM-v2020.10-stm32mp-r1-CONFIG.patch \
    \
    file://0099-Add-external-var-to-allow-build-of-new-devicetree-fi.patch \
"

U_BOOT_VERSION = "2020.10"
PV = "${U_BOOT_VERSION}.r1"

S = "${WORKDIR}/git"

# ---------------------------------
# Configure devupstream class usage
# ---------------------------------
BBCLASSEXTEND = "devupstream:target"

SRC_URI_class-devupstream = "git://github.com/STMicroelectronics/u-boot.git;protocol=https;branch=v${U_BOOT_VERSION}-stm32mp"
SRCREV_class-devupstream = "d2c99cb7e5b7dc74236f67fcced156956155cd71"

# ---------------------------------
# Configure default preference to manage dynamic selection between tarball and github
# ---------------------------------
STM32MP_SOURCE_SELECTION ?= "tarball"

DEFAULT_PREFERENCE = "${@bb.utils.contains('STM32MP_SOURCE_SELECTION', 'github', '-1', '1', d)}"
