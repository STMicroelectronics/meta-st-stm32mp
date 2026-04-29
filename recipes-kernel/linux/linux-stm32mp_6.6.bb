SUMMARY = "Linux STM32MP Kernel"
SECTION = "kernel"
LICENSE = "GPL-2.0-with-Linux-syscall-note"
LIC_FILES_CHKSUM = " \
    file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46 \
    file://LICENSES/preferred/GPL-2.0;md5=e6a75371ba4d16749254a51215d13f97 \
    file://LICENSES/exceptions/Linux-syscall-note;md5=6b0dff741019b948dfe290c05d6f361c \
    "

include linux-stm32mp.inc

CVE_PRODUCT = "linux_kernel linux:linux"

LINUX_VERSION = "6.6"
LINUX_SUBVERSION = ".116"
LINUX_TARBASE = "linux-${LINUX_VERSION}${LINUX_SUBVERSION}"
LINUX_TARNAME = "${LINUX_TARBASE}.tar.xz"

SRC_URI = "https://cdn.kernel.org/pub/linux/kernel/v6.x/${LINUX_TARNAME};name=kernel"

SRC_URI[kernel.sha256sum] = "a9a59742c29be284c205dc87cbe9b065f9688488132c8f5a6057a5539230a51d"

SRC_URI += " \
    file://${LINUX_VERSION}/${LINUX_VERSION}${LINUX_SUBVERSION}/0001-v6.6-stm32mp-r3.patch \
    "

LINUX_TARGET = "stm32mp"
LINUX_RELEASE = "r3"

PV = "${LINUX_VERSION}${LINUX_SUBVERSION}-${LINUX_TARGET}-${LINUX_RELEASE}"

ARCHIVER_ST_BRANCH = "v${LINUX_VERSION}-${LINUX_TARGET}"
ARCHIVER_ST_REVISION = "v${LINUX_VERSION}-${LINUX_TARGET}-${LINUX_RELEASE}"
ARCHIVER_COMMUNITY_BRANCH = "linux-${LINUX_VERSION}.y"
ARCHIVER_COMMUNITY_REVISION = "v${LINUX_VERSION}${LINUX_SUBVERSION}"

S = "${UNPACKDIR}/${LINUX_TARBASE}"

# ---------------------------------
# Configure devupstream class usage
# ---------------------------------
BBCLASSEXTEND = "devupstream:target"

SRC_URI:class-devupstream = "git://github.com/STMicroelectronics/linux.git;protocol=https;branch=${ARCHIVER_ST_BRANCH}"
SRCREV:class-devupstream = "80d29586bafa4734bad35a8f063919fdd79f5dec"
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
KERNEL_CONFIG_FRAGMENTS:arm = " \
    ${@bb.utils.contains('KERNEL_DEFCONFIG', 'defconfig', '${S}/arch/arm/configs/fragment-01-multiv7_cleanup.config', '', d)} \
    ${@bb.utils.contains('KERNEL_DEFCONFIG', 'defconfig', '${S}/arch/arm/configs/fragment-02-multiv7_addons.config', '', d)} \
    "

KERNEL_CONFIG_FRAGMENTS:aarch32 = " \
    ${@bb.utils.contains('KERNEL_DEFCONFIG', 'defconfig', '${S}/arch/arm/configs/mp2-fragment-01-defconfig-cleanup.config', '', d)} \
    ${@bb.utils.contains('KERNEL_DEFCONFIG', 'defconfig', '${S}/arch/arm/configs/mp2-fragment-02-defconfig-addons.config', '', d)} \
    ${@bb.utils.contains('KERNEL_DEFCONFIG', 'defconfig', '${S}/arch/arm/configs/mp2-fragment-03-multiv7-cleanup.config', '', d)} \
    ${@bb.utils.contains('KERNEL_DEFCONFIG', 'defconfig', '${S}/arch/arm/configs/mp2-fragment-04-multiv7-addons.config', '', d)} \
    "

KERNEL_CONFIG_FRAGMENTS:aarch64 = " \
    ${@bb.utils.contains('KERNEL_DEFCONFIG', 'defconfig', '${S}/arch/arm64/configs/fragment-01-defconfig-cleanup.config', '', d)} \
    ${@bb.utils.contains('KERNEL_DEFCONFIG', 'defconfig', '${S}/arch/arm64/configs/fragment-02-defconfig-addons.config', '', d)} \
    "

KERNEL_CONFIG_FRAGMENTS:append = " ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', '${UNPACKDIR}/fragments/${LINUX_VERSION}/fragment-03-systemd.config', '', d)} "
KERNEL_CONFIG_FRAGMENTS:append = " ${UNPACKDIR}/fragments/${LINUX_VERSION}/fragment-04-modules.config"
KERNEL_CONFIG_FRAGMENTS:append = " ${@oe.utils.ifelse(d.getVar('KERNEL_SIGN_ENABLE') == '1', '${UNPACKDIR}/fragments/features/${LINUX_VERSION}/optional-fragment-05-signature.config','')} "
KERNEL_CONFIG_FRAGMENTS:append = " ${@bb.utils.contains('MACHINE_FEATURES', 'efi', '${UNPACKDIR}/fragments/features/${LINUX_VERSION}/optional-fragment-07-efi.config', '', d)} "
KERNEL_CONFIG_FRAGMENTS:append:arm = " ${@bb.utils.contains('MACHINE_FEATURES', 'nosmp', '${UNPACKDIR}/fragments/features/${LINUX_VERSION}/optional-fragment-06-nosmp.config', '', d)} "


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

# add modules dependency
SRC_URI:append:arm = " file://stm32mp1-snd.conf;subdir=modprobe.d/"
SRC_URI:append:aarch32 = " file://stm32mp2_ucsi.conf;subdir=modprobe.d/"
SRC_URI:append:aarch64 = " file://stm32mp2_ucsi.conf;subdir=modprobe.d/"

# -------------------------------------------------------------
# Kernel Args
#
KERNEL_EXTRA_ARGS += "LOADADDR=${ST_KERNEL_LOADADDR}"
