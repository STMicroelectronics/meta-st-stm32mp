SUMMARY = "Linux STM32MP Kernel"
SECTION = "kernel"
LICENSE = "GPL-2.0-only"
#LIC_FILES_CHKSUM = "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"
LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

include linux-stm32mp.inc

LINUX_VERSION = "6.1"
LINUX_SUBVERSION = ".82"
LINUX_TARBASE = "linux-${LINUX_VERSION}${LINUX_SUBVERSION}"
LINUX_TARNAME = "${LINUX_TARBASE}.tar.xz"

SRC_URI = "https://cdn.kernel.org/pub/linux/kernel/v6.x/${LINUX_TARNAME};name=kernel"

SRC_URI[kernel.sha256sum] = "d150d2d9d416877668d8b56f75759f166168d192419eefaa942ed67225cbec06"

SRC_URI += " \
    file://${LINUX_VERSION}/${LINUX_VERSION}${LINUX_SUBVERSION}/0001-v6.1-stm32mp-r2.patch \
    "

LINUX_TARGET = "stm32mp"
LINUX_RELEASE = "r2"

PV = "${LINUX_VERSION}${LINUX_SUBVERSION}-${LINUX_TARGET}-${LINUX_RELEASE}"

ARCHIVER_ST_BRANCH = "v${LINUX_VERSION}-${LINUX_TARGET}"
ARCHIVER_ST_REVISION = "v${LINUX_VERSION}-${LINUX_TARGET}-${LINUX_RELEASE}"
ARCHIVER_COMMUNITY_BRANCH = "linux-${LINUX_VERSION}.y"
ARCHIVER_COMMUNITY_REVISION = "v${LINUX_VERSION}${LINUX_SUBVERSION}"

S = "${WORKDIR}/${LINUX_TARBASE}"

# ---------------------------------
# Configure devupstream class usage
# ---------------------------------
BBCLASSEXTEND = "devupstream:target"

SRC_URI:class-devupstream = "git://github.com/STMicroelectronics/linux.git;protocol=https;branch=${ARCHIVER_ST_BRANCH}"
SRCREV:class-devupstream = "7084443b1f4255944dc48190b3e95127e692dc2a"
#FIXME force the PV to avoid build issue:
#  do_package: ExpansionError('SRCPV', '${@bb.fetch2.get_srcrev(d)}', FetchError('SRCREV was used yet no valid SCM was found in SRC_URI', None))
PV:class-devupstream = "${LINUX_VERSION}${LINUX_SUBVERSION}-${LINUX_TARGET}.${SRCPV}"

# ---------------------------------
# Configure default preference to manage dynamic selection between tarball and github
# ---------------------------------
STM32MP_SOURCE_SELECTION ?= "tarball"

DEFAULT_PREFERENCE = "${@bb.utils.contains('STM32MP_SOURCE_SELECTION', 'github', '-1', '1', d)}"

# ---------------------------------
# Configure archiver use
# ---------------------------------
include ${@oe.utils.ifelse(d.getVar('ST_ARCHIVER_ENABLE') == '1', 'linux-stm32mp-archiver.inc','')}

# -------------------------------------------------------------
# Defconfig
#
KERNEL_DEFCONFIG        = "defconfig"
KERNEL_CONFIG_FRAGMENTS:stm32mp1common = "${@bb.utils.contains('KERNEL_DEFCONFIG', 'defconfig', '${S}/arch/arm/configs/fragment-01-multiv7_cleanup.config', '', d)}"
KERNEL_CONFIG_FRAGMENTS:append:stm32mp1common = " ${@bb.utils.contains('KERNEL_DEFCONFIG', 'defconfig', '${S}/arch/arm/configs/fragment-02-multiv7_addons.config', '', d)}"
KERNEL_CONFIG_FRAGMENTS:append:stm32mp1common = " ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', '${WORKDIR}/fragments/${LINUX_VERSION}/fragment-03-systemd.config', '', d)} "
KERNEL_CONFIG_FRAGMENTS:append:stm32mp1common = " ${WORKDIR}/fragments/${LINUX_VERSION}/fragment-04-modules.config"
KERNEL_CONFIG_FRAGMENTS:append:stm32mp1common = " ${@oe.utils.ifelse(d.getVar('KERNEL_SIGN_ENABLE') == '1', '${WORKDIR}/fragments/features/${LINUX_VERSION}/optional-fragment-05-signature.config','')} "
KERNEL_CONFIG_FRAGMENTS:append:stm32mp1common = " ${@bb.utils.contains('MACHINE_FEATURES', 'nosmp', '${WORKDIR}/fragments/features/${LINUX_VERSION}/optional-fragment-06-nosmp.config', '', d)} "
KERNEL_CONFIG_FRAGMENTS:append:stm32mp1common = " ${@bb.utils.contains('MACHINE_FEATURES', 'efi', '${WORKDIR}/fragments/features/${LINUX_VERSION}/optional-fragment-07-efi.config', '', d)} "

KERNEL_CONFIG_FRAGMENTS:stm32mp25common = "${@bb.utils.contains('KERNEL_DEFCONFIG', 'defconfig', '${S}/arch/arm64/configs/fragment-01-defconfig-cleanup.config', '', d)}"
KERNEL_CONFIG_FRAGMENTS:append:stm32mp25common = " ${@bb.utils.contains('KERNEL_DEFCONFIG', 'defconfig', '${S}/arch/arm64/configs/fragment-02-defconfig-addons.config', '', d)}"
KERNEL_CONFIG_FRAGMENTS:append:stm32mp25common = " ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', '${WORKDIR}/fragments/${LINUX_VERSION}/fragment-03-systemd.config', '', d)} "
KERNEL_CONFIG_FRAGMENTS:append:stm32mp25common = " ${WORKDIR}/fragments/${LINUX_VERSION}/fragment-04-modules.config"
KERNEL_CONFIG_FRAGMENTS:append:stm32mp25common = " ${@oe.utils.ifelse(d.getVar('KERNEL_SIGN_ENABLE') == '1', '${WORKDIR}/fragments/features/${LINUX_VERSION}/optional-fragment-05-signature.config','')} "
KERNEL_CONFIG_FRAGMENTS:append:stm32mp25common = " ${@bb.utils.contains('MACHINE_FEATURES', 'nosmp', '${WORKDIR}/fragments/features/${LINUX_VERSION}/optional-fragment-06-nosmp.config', '', d)} "
KERNEL_CONFIG_FRAGMENTS:append:stm32mp25common = " ${@bb.utils.contains('MACHINE_FEATURES', 'efi', '${WORKDIR}/fragments/features/${LINUX_VERSION}/optional-fragment-07-efi.config', '', d)} "

SRC_URI += "file://${LINUX_VERSION}/fragment-03-systemd.config;subdir=fragments"
SRC_URI += "file://${LINUX_VERSION}/fragment-04-modules.config;subdir=fragments"
SRC_URI += "file://${LINUX_VERSION}/optional-fragment-05-signature.config;subdir=fragments/features"
SRC_URI += "file://${LINUX_VERSION}/optional-fragment-06-nosmp.config;subdir=fragments/features"
SRC_URI += "file://${LINUX_VERSION}/optional-fragment-07-efi.config;subdir=fragments/features"

# Don't forget to add/del for devupstream
SRC_URI:class-devupstream += "file://${LINUX_VERSION}/fragment-03-systemd.config;subdir=fragments"
SRC_URI:class-devupstream += "file://${LINUX_VERSION}/fragment-04-modules.config;subdir=fragments"
SRC_URI:class-devupstream += "file://${LINUX_VERSION}/optional-fragment-05-signature.config;subdir=fragments/features"
SRC_URI:class-devupstream += "file://${LINUX_VERSION}/optional-fragment-06-nosmp.config;subdir=fragments/features"
SRC_URI:class-devupstream += "file://${LINUX_VERSION}/optional-fragment-07-efi.config;subdir=fragments/features"
# -------------------------------------------------------------
# Kernel Args
#
KERNEL_EXTRA_ARGS += "LOADADDR=${ST_KERNEL_LOADADDR}"
