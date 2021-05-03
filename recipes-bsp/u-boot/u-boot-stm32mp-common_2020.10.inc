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
    file://0006-ARM-v2020.10-stm32mp-r1.1.patch \
    file://0007-ARM-v2020.10-stm32mp-r2-MACHINE.patch \
    file://0008-ARM-v2020.10-stm32mp-r2-BOARD.patch \
    file://0009-ARM-v2020.10-stm32mp-r2-MISC-DRIVERS.patch \
    file://0010-ARM-v2020.10-stm32mp-r2-DEVICETREE.patch \
    file://0011-ARM-v2020.10-stm32mp-r2-CONFIG.patch \
    \
    file://0099-Add-external-var-to-allow-build-of-new-devicetree-fi.patch \
"

# debug and trace
SRC_URI += "${@bb.utils.contains('ST_UBOOT_DEBUG_TRACE', '1', '', 'file://0098-silent_mode.patch', d)}"

U_BOOT_VERSION = "v2020.10"
U_BOOT_SUBVERSION = "stm32mp"
U_BOOT_RELEASE = "r2"
PV = "${U_BOOT_VERSION}-${U_BOOT_SUBVERSION}-${U_BOOT_RELEASE}"

ARCHIVER_ST_BRANCH = "${U_BOOT_VERSION}-${U_BOOT_SUBVERSION}"
ARCHIVER_ST_REVISION = "${PV}"
ARCHIVER_COMMUNITY_BRANCH = "master"
ARCHIVER_COMMUNITY_REVISION = "${U_BOOT_VERSION}"

S = "${WORKDIR}/git"

# ---------------------------------
# Configure devupstream class usage
# ---------------------------------
BBCLASSEXTEND = "devupstream:target"

SRC_URI_class-devupstream = "git://github.com/STMicroelectronics/u-boot.git;protocol=https;branch=${ARCHIVER_ST_BRANCH}"
SRCREV_class-devupstream = "6fde82adf71ebad4b16d99f76ed69abfd9cfaf42"

# ---------------------------------
# Configure default preference to manage dynamic selection between tarball and github
# ---------------------------------
STM32MP_SOURCE_SELECTION ?= "tarball"

DEFAULT_PREFERENCE = "${@bb.utils.contains('STM32MP_SOURCE_SELECTION', 'github', '-1', '1', d)}"
