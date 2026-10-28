SUMMARY = "Linux STM32MP Kernel"
SECTION = "kernel"
LICENSE = "GPL-2.0-with-Linux-syscall-note"
LIC_FILES_CHKSUM = " \
    file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46 \
    file://LICENSES/preferred/GPL-2.0;md5=a362c03f8df4e23f8a155e1cb2c45b2a \
    file://LICENSES/exceptions/Linux-syscall-note;md5=6b0dff741019b948dfe290c05d6f361c \
    "

include linux-stm32mp.inc

CVE_PRODUCT = "linux_kernel linux:linux"
include cve-exclusion-6.18.inc


LINUX_VERSION = "6.18"
LINUX_SUBVERSION = ".37"
LINUX_TARBASE = "linux-${LINUX_VERSION}${LINUX_SUBVERSION}"
LINUX_TARNAME = "${LINUX_TARBASE}.tar.xz"

SRC_URI = "https://cdn.kernel.org/pub/linux/kernel/v6.x/${LINUX_TARNAME};name=kernel"
SRC_URI[kernel.sha256sum] = "a83cd200e6646db52866b8309e9137b9e9048b613cbda10ced2b811aae125255"

SRC_URI += " \
    file://${LINUX_VERSION}/${LINUX_VERSION}${LINUX_SUBVERSION}/0001-v6.18-stm32mp-r1.patch \
    "

LINUX_TARGET = "stm32mp"
LINUX_RELEASE = "r1"

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
SRCREV:class-devupstream = "3056405e0479330b0889c2a971452bfd10014b70"
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

KERNEL_CONFIG_FRAGMENTS:append = " ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', '${UNPACKDIR}/fragments/fragment-03-systemd.config', '', d)} "
KERNEL_CONFIG_FRAGMENTS:append = " ${UNPACKDIR}/fragments/fragment-04-modules.config"
KERNEL_CONFIG_FRAGMENTS:append = " ${@oe.utils.ifelse(d.getVar('KERNEL_SIGN_ENABLE') == '1', '${UNPACKDIR}/features/optional-fragment-05-signature.config','')} "
KERNEL_CONFIG_FRAGMENTS:append = " ${@bb.utils.contains('MACHINE_FEATURES', 'efi', '${UNPACKDIR}/features/optional-fragment-07-efi.config', '', d)} "
KERNEL_CONFIG_FRAGMENTS:append:stm32mp1common = " ${@bb.utils.contains('MACHINE_FEATURES', 'efi', '${UNPACKDIR}/features/optional-fragment-07-efi-stm32mp1.config', '', d)} "
KERNEL_CONFIG_FRAGMENTS:append:arm = " ${@bb.utils.contains('MACHINE_FEATURES', 'nosmp', '${UNPACKDIR}/features/optional-fragment-06-nosmp.config', '', d)} "
KERNEL_CONFIG_FRAGMENTS:append:aarch64 = " ${@bb.utils.contains('MACHINE_FEATURES', 'gpu', '${UNPACKDIR}/features/optional-fragment-08-disable-etnaviv.config', '', d)} "
KERNEL_CONFIG_FRAGMENTS:append:aarch64 = " ${@bb.utils.contains('MACHINE_FEATURES', 'nogpu', '${UNPACKDIR}/features/optional-fragment-08-disable-etnaviv.config', '', d)} "
KERNEL_CONFIG_FRAGMENTS:append:aarch32 = " ${@bb.utils.contains('MACHINE_FEATURES', 'gpu', '${UNPACKDIR}/features/optional-fragment-08-disable-etnaviv.config', '', d)} "
KERNEL_CONFIG_FRAGMENTS:append:aarch32 = " ${@bb.utils.contains('MACHINE_FEATURES', 'nogpu', '${UNPACKDIR}/features/optional-fragment-08-disable-etnaviv.config', '', d)} "

SRC_URI += "file://fragments/fragment-03-systemd.config"
SRC_URI += "file://fragments/fragment-04-modules.config"
SRC_URI += "file://features/optional-fragment-05-signature.config"
SRC_URI += "file://features/optional-fragment-06-nosmp.config"
SRC_URI += "file://features/optional-fragment-07-efi.config"
SRC_URI += "file://features/optional-fragment-07-efi-stm32mp1.config"
SRC_URI += "file://features/optional-fragment-08-disable-etnaviv.config"

# Don't forget to add/del for devupstream
SRC_URI:class-devupstream += "file://fragments/fragment-03-systemd.config"
SRC_URI:class-devupstream += "file://fragments/fragment-04-modules.config"
SRC_URI:class-devupstream += "file://features/optional-fragment-05-signature.config"
SRC_URI:class-devupstream += "file://features/optional-fragment-06-nosmp.config"
SRC_URI:class-devupstream += "file://features/optional-fragment-07-efi.config"
SRC_URI:class-devupstream += "file://features/optional-fragment-08-disable-etnaviv.config"

# add modules dependency
SRC_URI:append:arm = " file://stm32mp1-snd.conf;subdir=modprobe.d/"
SRC_URI:append:aarch32 = " file://stm32mp2_ucsi.conf;subdir=modprobe.d/"
SRC_URI:append:aarch64 = " file://stm32mp2_ucsi.conf;subdir=modprobe.d/"
SRC_URI:append:aarch32 = " file://stm32mp2_uio.conf;subdir=modprobe.d/"
SRC_URI:append:aarch64 = " file://stm32mp2_uio.conf;subdir=modprobe.d/"
SRC_URI:append:arm = " file://stm32mp_uvcvideo.conf;subdir=modprobe.d/"
SRC_URI:append:aarch64 = " file://stm32mp_uvcvideo.conf;subdir=modprobe.d/"

# -------------------------------------------------------------
# Kernel Args
#
KERNEL_EXTRA_ARGS += "LOADADDR=${ST_KERNEL_LOADADDR}"
