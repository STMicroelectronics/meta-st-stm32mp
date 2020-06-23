SUMMARY = "Linux STM32MP Kernel"
SECTION = "kernel"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"

include linux-stm32mp.inc

LINUX_VERSION = "5.4"
LINUX_SUBVERSION = "31"
SRC_URI = "https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-${LINUX_VERSION}.${LINUX_SUBVERSION}.tar.xz;name=kernel"
SRC_URI[kernel.sha256sum] = "a11083f8f809887f6a0f8d4467532385b99418f17998fe6e837807491c276eeb"

SRC_URI += " \
    file://${LINUX_VERSION}/${LINUX_VERSION}.${LINUX_SUBVERSION}/0001-ARM-stm32mp1-r1-MACHINE.patch \
    file://${LINUX_VERSION}/${LINUX_VERSION}.${LINUX_SUBVERSION}/0002-ARM-stm32mp1-r1-CPUFREQ.patch \
    file://${LINUX_VERSION}/${LINUX_VERSION}.${LINUX_SUBVERSION}/0003-ARM-stm32mp1-r1-CRYPTO.patch \
    file://${LINUX_VERSION}/${LINUX_VERSION}.${LINUX_SUBVERSION}/0004-ARM-stm32mp1-r1-RNG-DEBUG-NVMEM.patch \
    file://${LINUX_VERSION}/${LINUX_VERSION}.${LINUX_SUBVERSION}/0005-ARM-stm32mp1-r1-CLOCK.patch \
    file://${LINUX_VERSION}/${LINUX_VERSION}.${LINUX_SUBVERSION}/0006-ARM-stm32mp1-r1-DMA.patch \
    file://${LINUX_VERSION}/${LINUX_VERSION}.${LINUX_SUBVERSION}/0007-ARM-stm32mp1-r1-DRM.patch \
    file://${LINUX_VERSION}/${LINUX_VERSION}.${LINUX_SUBVERSION}/0008-ARM-stm32mp1-r1-HWSPINLOCK.patch \
    file://${LINUX_VERSION}/${LINUX_VERSION}.${LINUX_SUBVERSION}/0009-ARM-stm32mp1-r1-I2C-IIO-IRQCHIP.patch \
    file://${LINUX_VERSION}/${LINUX_VERSION}.${LINUX_SUBVERSION}/0010-ARM-stm32mp1-r1-MAILBOX-REMOTEPROC-RPMSG.patch \
    file://${LINUX_VERSION}/${LINUX_VERSION}.${LINUX_SUBVERSION}/0011-ARM-stm32mp1-r1-RESET-RTC-WATCHDOG.patch \
    file://${LINUX_VERSION}/${LINUX_VERSION}.${LINUX_SUBVERSION}/0012-ARM-stm32mp1-r1-MEDIA-SOC-THERMAL.patch \
    file://${LINUX_VERSION}/${LINUX_VERSION}.${LINUX_SUBVERSION}/0013-ARM-stm32mp1-r1-MFD.patch \
    file://${LINUX_VERSION}/${LINUX_VERSION}.${LINUX_SUBVERSION}/0014-ARM-stm32mp1-r1-MMC-NAND.patch \
    file://${LINUX_VERSION}/${LINUX_VERSION}.${LINUX_SUBVERSION}/0015-ARM-stm32mp1-r1-NET-TTY.patch \
    file://${LINUX_VERSION}/${LINUX_VERSION}.${LINUX_SUBVERSION}/0016-ARM-stm32mp1-r1-PHY-USB.patch \
    file://${LINUX_VERSION}/${LINUX_VERSION}.${LINUX_SUBVERSION}/0017-ARM-stm32mp1-r1-PINCTRL-REGULATOR-SPI-PWM.patch \
    file://${LINUX_VERSION}/${LINUX_VERSION}.${LINUX_SUBVERSION}/0018-ARM-stm32mp1-r1-SOUND.patch \
    file://${LINUX_VERSION}/${LINUX_VERSION}.${LINUX_SUBVERSION}/0019-ARM-stm32mp1-r1-MISC.patch \
    file://${LINUX_VERSION}/${LINUX_VERSION}.${LINUX_SUBVERSION}/0020-ARM-stm32mp1-r1-DEVICETREE.patch \
    file://${LINUX_VERSION}/${LINUX_VERSION}.${LINUX_SUBVERSION}/0021-ARM-stm32mp1-r1-CONFIG.patch \
    file://${LINUX_VERSION}/${LINUX_VERSION}.${LINUX_SUBVERSION}/0022-ARM-stm32mp1-r1-POWER.patch \
    file://${LINUX_VERSION}/${LINUX_VERSION}.${LINUX_SUBVERSION}/0023-ARM-stm32mp1-r1-PERF.patch \
"

PV = "${LINUX_VERSION}.${LINUX_SUBVERSION}"

S = "${WORKDIR}/linux-${LINUX_VERSION}.${LINUX_SUBVERSION}"

# ---------------------------------
# Configure devupstream class usage
# ---------------------------------
BBCLASSEXTEND = "devupstream:target"

SRC_URI_class-devupstream = "git://github.com/STMicroelectronics/linux.git;protocol=https;branch=v${LINUX_VERSION}-stm32mp;name=linux"
SRCREV_class-devupstream = "b8663f5fdb5cfd6f243b72c9fac82c24b2594294"
SRCREV_FORMAT_class-devupstream = "linux"
PV_class-devupstream = "${LINUX_VERSION}+github+${SRCPV}"

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
KERNEL_CONFIG_FRAGMENTS = "${@bb.utils.contains('KERNEL_DEFCONFIG', 'defconfig', '${S}/arch/arm/configs/fragment-01-multiv7_cleanup.config', '', d)}"
KERNEL_CONFIG_FRAGMENTS += "${@bb.utils.contains('KERNEL_DEFCONFIG', 'defconfig', '${S}/arch/arm/configs/fragment-02-multiv7_addons.config', '', d)}"
KERNEL_CONFIG_FRAGMENTS += "${@bb.utils.contains('DISTRO_FEATURES', 'systemd', '${WORKDIR}/fragments/5.4/fragment-03-systemd.config', '', d)} "
KERNEL_CONFIG_FRAGMENTS += "${@bb.utils.contains('COMBINED_FEATURES', 'optee', '${WORKDIR}/fragments/5.4/fragment-04-optee.config', '', d)}"
KERNEL_CONFIG_FRAGMENTS += "${WORKDIR}/fragments/5.4/fragment-05-modules.config"
KERNEL_CONFIG_FRAGMENTS += "${@oe.utils.ifelse(d.getVar('KERNEL_SIGN_ENABLE') == '1', '${WORKDIR}/fragments/5.4/fragment-06-signature.config','')} "

SRC_URI += "file://${LINUX_VERSION}/fragment-03-systemd.config;subdir=fragments"
SRC_URI += "file://${LINUX_VERSION}/fragment-04-optee.config;subdir=fragments"
SRC_URI += "file://${LINUX_VERSION}/fragment-05-modules.config;subdir=fragments"
SRC_URI += "file://${LINUX_VERSION}/fragment-06-signature.config;subdir=fragments"

# Don't forget to add/del for devupstream
#SRC_URI_class-devupstream += " file://${LINUX_VERSION}/fragment-03-systemd.config;subdir=fragments "
#SRC_URI_class-devupstream += " file://${LINUX_VERSION}/fragment-04-optee.config;subdir=fragments "
#SRC_URI_class-devupstream += " file://${LINUX_VERSION}/fragment-05-modules.config;subdir=fragments "

# -------------------------------------------------------------
# Kernel Args
#
KERNEL_EXTRA_ARGS += "LOADADDR=${ST_KERNEL_LOADADDR}"
