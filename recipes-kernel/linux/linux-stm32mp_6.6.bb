SUMMARY = "Linux STM32MP Kernel"
SECTION = "kernel"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

include linux-stm32mp.inc

LINUX_VERSION = "6.6"
LINUX_SUBVERSION = ".48"
LINUX_TARBASE = "linux-${LINUX_VERSION}${LINUX_SUBVERSION}"
LINUX_TARNAME = "${LINUX_TARBASE}.tar.xz"

SRC_URI = "https://cdn.kernel.org/pub/linux/kernel/v6.x/${LINUX_TARNAME};name=kernel"

SRC_URI[kernel.sha256sum] = "6b16df7b2aba3116b78fdfd8aea0b6cd7abe8f0cb699b04a66d3169141772029"

SRC_URI += " \
    file://${LINUX_VERSION}/${LINUX_VERSION}${LINUX_SUBVERSION}/0001-v6.6-stm32mp-r1.2.patch \
    "

LINUX_TARGET = "stm32mp"
LINUX_RELEASE = "r1.2"

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
SRCREV:class-devupstream = "57d6654e68728d6b6776e688a0f8ece6ecfc1209"
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

KERNEL_CONFIG_FRAGMENTS:stm32mp2common = "${@bb.utils.contains('KERNEL_DEFCONFIG', 'defconfig', '${S}/arch/arm64/configs/fragment-01-defconfig-cleanup.config', '', d)}"
KERNEL_CONFIG_FRAGMENTS:append:stm32mp2common = " ${@bb.utils.contains('KERNEL_DEFCONFIG', 'defconfig', '${S}/arch/arm64/configs/fragment-02-defconfig-addons.config', '', d)}"
KERNEL_CONFIG_FRAGMENTS:append:stm32mp2common = " ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', '${WORKDIR}/fragments/${LINUX_VERSION}/fragment-03-systemd.config', '', d)} "
KERNEL_CONFIG_FRAGMENTS:append:stm32mp2common = " ${WORKDIR}/fragments/${LINUX_VERSION}/fragment-04-modules.config"
KERNEL_CONFIG_FRAGMENTS:append:stm32mp2common = " ${@oe.utils.ifelse(d.getVar('KERNEL_SIGN_ENABLE') == '1', '${WORKDIR}/fragments/features/${LINUX_VERSION}/optional-fragment-05-signature.config','')} "
KERNEL_CONFIG_FRAGMENTS:append:stm32mp2common = " ${@bb.utils.contains('MACHINE_FEATURES', 'nosmp', '${WORKDIR}/fragments/features/${LINUX_VERSION}/optional-fragment-06-nosmp.config', '', d)} "
KERNEL_CONFIG_FRAGMENTS:append:stm32mp2common = " ${@bb.utils.contains('MACHINE_FEATURES', 'efi', '${WORKDIR}/fragments/features/${LINUX_VERSION}/optional-fragment-07-efi.config', '', d)} "

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
