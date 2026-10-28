# Adaptation from u-boot-common_${PV}.inc

HOMEPAGE = "http://www.denx.de/wiki/U-Boot/WebHome"
SECTION = "bootloaders"

LICENSE = "GPL-2.0-or-later"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=2ca5f2c35c8cc335f0a19756634782f1"

CVE_PRODUCT = "denx:u-boot"

DEPENDS += "dtc-native bc-native"
DEPENDS += "flex-native bison-native"
DEPENDS += "python3-setuptools-native"
DEPENDS += "python3-dtc-native"
DEPENDS += "gnutls-native"

SRC_URI = "git://github.com/u-boot/u-boot.git;protocol=https;branch=master"
SRCREV  = "88dc2788777babfd6322fa655df549a019aa1e69"

SRC_URI += "\
    file://0001-v2026.04-stm32mp-r1.patch \
"

UBOOT_CONFIG_ADDONS_FRAGMENTS = ""
# debug and trace
SRC_URI += " ${@bb.utils.contains('ST_UBOOT_DEBUG_TRACE', '1', '', 'file://0098-silent_mode.patch', d)} "
SRC_URI += "${@bb.utils.contains('ST_UBOOT_DEBUG_TRACE', '1', '', 'file://fragment-01-silent_mode.cfg;subdir=fragments/features', d)}"

# optional config
SRC_URI += " file://fragment-02-signature.fragment;subdir=fragments/features"
UBOOT_CONFIG_ADDONS_FRAGMENTS += "${@bb.utils.contains('UBOOT_SIGN_ENABLE', '1', '${UNPACKDIR}/fragments/features/fragment-02-signature.fragment', '', d)}"

# efi config
SRC_URI += "file://fragment-03-efi.fragment;subdir=fragments/features"
UBOOT_CONFIG_ADDONS_FRAGMENTS += "${@bb.utils.contains('MACHINE_FEATURES', 'efi', '${UNPACKDIR}/fragments/features/fragment-03-efi.fragment', '', d)}"

# M33TD: no splashscreen
SRC_URI += "file://fragment-06-m33td-no-splashscreen.fragment;subdir=fragments/features"
UBOOT_CONFIG_ADDONS_FRAGMENTS += "${@bb.utils.contains('MACHINE_FEATURES', 'm33td', '${UNPACKDIR}/fragments/features/fragment-06-m33td-no-splashscreen.fragment', '', d)}"

# firmware update ab
SRC_URI += "file://fragment-07-firmware-update-ab.fragment;subdir=fragments/features"
UBOOT_CONFIG_ADDONS_FRAGMENTS += "${@bb.utils.contains('MACHINE_FEATURES', 'fw-update-ab', '${UNPACKDIR}/fragments/features/fragment-07-firmware-update-ab.fragment', '', d)}"

U_BOOT_VERSION = "v2026.04"
U_BOOT_SUBVERSION = "stm32mp"
U_BOOT_RELEASE = "r1"

PV = "${U_BOOT_VERSION}-${U_BOOT_SUBVERSION}-${U_BOOT_RELEASE}"

ARCHIVER_ST_BRANCH = "${U_BOOT_VERSION}-${U_BOOT_SUBVERSION}"
ARCHIVER_ST_REVISION = "${PV}"
ARCHIVER_COMMUNITY_BRANCH = "master"
ARCHIVER_COMMUNITY_REVISION = "${U_BOOT_VERSION}"

# ---------------------------------
# Configure devupstream class usage
# ---------------------------------
BBCLASSEXTEND = "devupstream:target"

SRC_URI:class-devupstream = "git://github.com/STMicroelectronics/u-boot.git;protocol=https;branch=${ARCHIVER_ST_BRANCH}"
SRCREV:class-devupstream = "89a83818f9c0412006b549563511a46acb1429ba"

# ---------------------------------
# Configure default preference to manage dynamic selection between tarball and github
# ---------------------------------
STM32MP_SOURCE_SELECTION ?= "tarball"

DEFAULT_PREFERENCE = "${@bb.utils.contains('STM32MP_SOURCE_SELECTION', 'github', '-1', '1', d)}"
