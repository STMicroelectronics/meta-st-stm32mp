# Adaptation from u-boot-common_${PV}.inc

HOMEPAGE = "http://www.denx.de/wiki/U-Boot/WebHome"
SECTION = "bootloaders"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"

DEPENDS += "dtc-native bc-native"
DEPENDS += "flex-native bison-native"

COMPATIBLE_MACHINE = "(stm32mpcommon)"

SRC_URI = "git://github.com/u-boot/u-boot.git;protocol=https"
SRCREV  = "0b0c6af38738f2c132cfd41a240889acaa031c8f"

SRC_URI += "\
    file://0001-ARM-v2020.01-stm32mp-r1-MACHINE.patch \
    file://0002-ARM-v2020.01-stm32mp-r1-BOARD.patch \
    file://0003-ARM-v2020.01-stm32mp-r1-MISC-DRIVERS.patch \
    file://0004-ARM-v2020.01-stm32mp-r1-DEVICETREE.patch \
    file://0005-ARM-v2020.01-stm32mp-r1-CONFIG.patch \
    \
    file://0006-ARM-v2020.01-stm32mp-r2-DEVICETREE.patch \
    file://0007-ARM-v2020.01-stm32mp-r2-MACHINE.patch \
    file://0008-ARM-v2020.01-stm32mp-r2-BOARD.patch \
    file://0009-ARM-v2020.01-stm32mp-r2-MISC-DRIVERS.patch \
    file://0010-ARM-v2020.01-stm32mp-r2-CONFIG.patch \
    file://0011-ARM-v2020.01-stm32mp-r2.2.patch \
    \
    file://0099-Add-external-var-to-allow-build-of-new-devicetree-fi.patch \
    file://remove-redundant-yyloc-global.patch \
"

U_BOOT_VERSION = "2020.01"
PV = "${U_BOOT_VERSION}.r2"

S = "${WORKDIR}/git"

# ---------------------------------
# Configure devupstream class usage
# ---------------------------------
BBCLASSEXTEND = "devupstream:target"

SRC_URI_class-devupstream = "git://github.com/STMicroelectronics/u-boot.git;protocol=https;branch=v${U_BOOT_VERSION}-stm32mp"
SRCREV_class-devupstream = "d543131c5835b2ca010d8169a06dbca3014355fd"

# ---------------------------------
# Configure default preference to manage dynamic selection between tarball and github
# ---------------------------------
STM32MP_SOURCE_SELECTION ?= "tarball"

DEFAULT_PREFERENCE = "${@bb.utils.contains('STM32MP_SOURCE_SELECTION', 'github', '-1', '1', d)}"
