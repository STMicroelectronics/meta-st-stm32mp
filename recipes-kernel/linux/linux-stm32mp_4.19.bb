SUMMARY = "Linux STM32MP Kernel"
SECTION = "kernel"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"

include linux-stm32mp.inc

SRC_URI = "https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.19.94.tar.xz"
SRC_URI[md5sum] = "3f5621e9b463a3574618f3edfe438e4a"
SRC_URI[sha256sum] = "c62a10a75a7c4213e41287040e7c7509b7d42117d6830feb7dfe505949fa7467"

SRC_URI += " \
    file://${LINUX_VERSION}/4.19.94/0001-ARM-stm32mp1-r3-MACHINE.patch \
    file://${LINUX_VERSION}/4.19.94/0002-ARM-stm32mp1-r3-CPUFREQ.patch \
    file://${LINUX_VERSION}/4.19.94/0003-ARM-stm32mp1-r3-CRYPTO.patch \
    file://${LINUX_VERSION}/4.19.94/0004-ARM-stm32mp1-r3-BLUETOOTH-CHAR.patch \
    file://${LINUX_VERSION}/4.19.94/0005-ARM-stm32mp1-r3-CLOCK.patch \
    file://${LINUX_VERSION}/4.19.94/0006-ARM-stm32mp1-r3-DMA.patch \
    file://${LINUX_VERSION}/4.19.94/0007-ARM-stm32mp1-r3-DRM.patch \
    file://${LINUX_VERSION}/4.19.94/0008-ARM-stm32mp1-r3-GPIO.patch \
    file://${LINUX_VERSION}/4.19.94/0009-ARM-stm32mp1-r3-HWSPINLOCK.patch \
    file://${LINUX_VERSION}/4.19.94/0010-ARM-stm32mp1-r3-HWTRACING-I2C.patch \
    file://${LINUX_VERSION}/4.19.94/0011-ARM-stm32mp1-r3-IIO.patch \
    file://${LINUX_VERSION}/4.19.94/0012-ARM-stm32mp1-r3-INPUT-IRQ-Mailbox.patch \
    file://${LINUX_VERSION}/4.19.94/0013-ARM-stm32mp1-r3-MEDIA.patch \
    file://${LINUX_VERSION}/4.19.94/0014-ARM-stm32mp1-r3-MFD.patch \
    file://${LINUX_VERSION}/4.19.94/0015-ARM-stm32mp1-r3-MMC-MTD.patch \
    file://${LINUX_VERSION}/4.19.94/0016-ARM-stm32mp1-r3-NET.patch \
    file://${LINUX_VERSION}/4.19.94/0017-ARM-stm32mp1-r3-NVMEM.patch \
    file://${LINUX_VERSION}/4.19.94/0018-ARM-stm32mp1-r3-PERF.patch \
    file://${LINUX_VERSION}/4.19.94/0019-ARM-stm32mp1-r3-PHY-PINCTRL-PWM.patch \
    file://${LINUX_VERSION}/4.19.94/0020-ARM-stm32mp1-r3-REGULATOR.patch \
    file://${LINUX_VERSION}/4.19.94/0021-ARM-stm32mp1-r3-REMOTEPROC-RPMSG-RESET.patch \
    file://${LINUX_VERSION}/4.19.94/0022-ARM-stm32mp1-r3-RTC.patch \
    file://${LINUX_VERSION}/4.19.94/0023-ARM-stm32mp1-r3-SOC.patch \
    file://${LINUX_VERSION}/4.19.94/0024-ARM-stm32mp1-r3-SPI.patch \
    file://${LINUX_VERSION}/4.19.94/0025-ARM-stm32mp1-r3-THERMAL.patch \
    file://${LINUX_VERSION}/4.19.94/0026-ARM-stm32mp1-r3-TTY-USB.patch \
    file://${LINUX_VERSION}/4.19.94/0027-ARM-stm32mp1-r3-WATCHDOG.patch \
    file://${LINUX_VERSION}/4.19.94/0028-ARM-stm32mp1-r3-SOUND.patch \
    file://${LINUX_VERSION}/4.19.94/0029-ARM-stm32mp1-r3-MISC.patch \
    file://${LINUX_VERSION}/4.19.94/0030-ARM-stm32mp1-r3-DEVICETREE.patch \
    file://${LINUX_VERSION}/4.19.94/0031-ARM-stm32mp1-r3-DEFCONFIG.patch \
    "

LINUX_VERSION = "4.19"

PV = "${LINUX_VERSION}"

S = "${WORKDIR}/linux-4.19.94"

# ---------------------------------
# Configure devupstream class usage
# ---------------------------------
BBCLASSEXTEND = "devupstream:target"

SRC_URI_class-devupstream = "git://github.com/STMicroelectronics/linux.git;protocol=https;branch=v${LINUX_VERSION}-stm32mp;name=linux"
SRCREV_class-devupstream = "1cb30cb5ffc29a53ec2031b6a29878ddd266516c"
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
KERNEL_DEFCONFIG        = "multi_v7_defconfig"
KERNEL_CONFIG_FRAGMENTS = "${@bb.utils.contains('KERNEL_DEFCONFIG', 'multi_v7_defconfig', '${S}/arch/arm/configs/fragment-01-multiv7_cleanup.config', '', d)}"
KERNEL_CONFIG_FRAGMENTS += "${@bb.utils.contains('KERNEL_DEFCONFIG', 'multi_v7_defconfig', '${S}/arch/arm/configs/fragment-02-multiv7_addons.config', '', d)}"
KERNEL_CONFIG_FRAGMENTS += "${@bb.utils.contains('DISTRO_FEATURES', 'systemd', '${WORKDIR}/fragments/4.19/fragment-03-systemd.config', '', d)} "
KERNEL_CONFIG_FRAGMENTS += "${@bb.utils.contains('COMBINED_FEATURES', 'optee', '${WORKDIR}/fragments/4.19/fragment-04-optee.config', '', d)}"
KERNEL_CONFIG_FRAGMENTS += "${WORKDIR}/fragments/4.19/fragment-05-modules.config"

SRC_URI += "file://4.19/fragment-03-systemd.config;subdir=fragments"
SRC_URI += "file://4.19/fragment-04-optee.config;subdir=fragments"
SRC_URI += "file://4.19/fragment-05-modules.config;subdir=fragments"
# Don't forget to add/del for devupstream
SRC_URI_class-devupstream += " file://4.19/fragment-03-systemd.config;subdir=fragments "
SRC_URI_class-devupstream += " file://4.19/fragment-04-optee.config;subdir=fragments "
SRC_URI_class-devupstream += " file://4.19/fragment-05-modules.config;subdir=fragments "

# -------------------------------------------------------------
# Kernel Args
#
KERNEL_EXTRA_ARGS += "LOADADDR=${ST_KERNEL_LOADADDR}"
