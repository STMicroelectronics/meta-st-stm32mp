# Adaptation from u-boot-common_${PV}.inc

HOMEPAGE = "http://www.denx.de/wiki/U-Boot/WebHome"
SECTION = "bootloaders"

LICENSE = "GPL-2.0-or-later"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=5a7450c57ffe5ae63fd732446b988025"

DEPENDS += "dtc-native bc-native"
DEPENDS += "flex-native bison-native"
DEPENDS += "python3-setuptools-native"

COMPATIBLE_MACHINE = "(stm32mpcommon)"

SRC_URI = "git://github.com/u-boot/u-boot.git;protocol=https;branch=master"
SRCREV  = "d80bb749fab53da72c4a0e09b8c2d2aaa3103c91"

SRC_URI += "\
    file://0001-ARM-v2021.10-stm32mp-r1-MACHINE.patch \
    file://0002-ARM-v2021.10-stm32mp-r1-BOARD.patch \
    file://0003-ARM-v2021.10-stm32mp-r1-MISC-DRIVERS.patch \
    file://0004-ARM-v2021.10-stm32mp-r1-DEVICETREE.patch \
    file://0005-ARM-v2021.10-stm32mp-r1-CONFIG.patch \
    file://0006-ARM-v2021.10-stm32mp-r2-MACHINE.patch \
    file://0007-ARM-v2021.10-stm32mp-r2-BOARD.patch \
    file://0008-ARM-v2021.10-stm32mp-r2-MISC-DRIVERS.patch \
    file://0009-ARM-v2021.10-stm32mp-r2-DEVICETREE.patch \
    \
    file://0099-Add-external-var-to-allow-build-of-new-devicetree-fi.patch \
"

# debug and trace
SRC_URI += "${@bb.utils.contains('ST_UBOOT_DEBUG_TRACE', '1', '', 'file://0098-silent_mode.patch', d)}"

U_BOOT_VERSION = "v2021.10"
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

SRC_URI:class-devupstream = "git://github.com/STMicroelectronics/u-boot.git;protocol=https;branch=${ARCHIVER_ST_BRANCH}"
SRCREV:class-devupstream = "3984366f6997c680b8c6ccc82d50e77a6e1cccf2"

# ---------------------------------
# Configure default preference to manage dynamic selection between tarball and github
# ---------------------------------
STM32MP_SOURCE_SELECTION ?= "tarball"

DEFAULT_PREFERENCE = "${@bb.utils.contains('STM32MP_SOURCE_SELECTION', 'github', '-1', '1', d)}"
