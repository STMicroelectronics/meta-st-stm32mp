SUMMARY = "Linux STM32MP Kernel"
SECTION = "kernel"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"

include linux-stm32mp.inc

SRC_URI = "https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.19.49.tar.xz"
SRC_URI[md5sum] = "0cb9baf0f5ed8f56d42cccc508d841b0"
SRC_URI[sha256sum] = "92d920b3973c0dbca5516271afa405be6e5822a9b831df8c085f9c9eb838bbcd"

SRC_URI += " \
    file://${LINUX_VERSION}/4.19.49/0001-ARM-stm32mp1-r2-MACHINE.patch \
    file://${LINUX_VERSION}/4.19.49/0002-ARM-stm32mp1-r2-CRYPTO.patch \
    file://${LINUX_VERSION}/4.19.49/0003-ARM-stm32mp1-r2-BLUETOOTH-CHAR.patch \
    file://${LINUX_VERSION}/4.19.49/0004-ARM-stm32mp1-r2-CLOCK.patch \
    file://${LINUX_VERSION}/4.19.49/0005-ARM-stm32mp1-r2-DMA.patch \
    file://${LINUX_VERSION}/4.19.49/0006-ARM-stm32mp1-r2-DRM.patch \
    file://${LINUX_VERSION}/4.19.49/0007-ARM-stm32mp1-r2-GPIO.patch \
    file://${LINUX_VERSION}/4.19.49/0008-ARM-stm32mp1-r2-HWSPINLOCK.patch \
    file://${LINUX_VERSION}/4.19.49/0009-ARM-stm32mp1-r2-HWTRACING-I2C.patch \
    file://${LINUX_VERSION}/4.19.49/0010-ARM-stm32mp1-r2-IIO.patch \
    file://${LINUX_VERSION}/4.19.49/0011-ARM-stm32mp1-r2-INPUT-IRQ-Mailbox.patch \
    file://${LINUX_VERSION}/4.19.49/0012-ARM-stm32mp1-r2-MEDIA.patch \
    file://${LINUX_VERSION}/4.19.49/0013-ARM-stm32mp1-r2-MFD.patch \
    file://${LINUX_VERSION}/4.19.49/0014-ARM-stm32mp1-r2-MMC-MTD.patch \
    file://${LINUX_VERSION}/4.19.49/0015-ARM-stm32mp1-r2-NET.patch \
    file://${LINUX_VERSION}/4.19.49/0016-ARM-stm32mp1-r2-NVMEM.patch \
    file://${LINUX_VERSION}/4.19.49/0017-ARM-stm32mp1-r2-PERF.patch \
    file://${LINUX_VERSION}/4.19.49/0018-ARM-stm32mp1-r2-PHY-PINCTRL-PWM.patch \
    file://${LINUX_VERSION}/4.19.49/0019-ARM-stm32mp1-r2-REGULATOR.patch \
    file://${LINUX_VERSION}/4.19.49/0020-ARM-stm32mp1-r2-REMOTEPROC-RPMSG-RESET.patch \
    file://${LINUX_VERSION}/4.19.49/0021-ARM-stm32mp1-r2-RTC.patch \
    file://${LINUX_VERSION}/4.19.49/0022-ARM-stm32mp1-r2-SOC.patch \
    file://${LINUX_VERSION}/4.19.49/0023-ARM-stm32mp1-r2-SPI.patch \
    file://${LINUX_VERSION}/4.19.49/0024-ARM-stm32mp1-r2-THERMAL.patch \
    file://${LINUX_VERSION}/4.19.49/0025-ARM-stm32mp1-r2-TTY-USB.patch \
    file://${LINUX_VERSION}/4.19.49/0026-ARM-stm32mp1-r2-WATCHDOG.patch \
    file://${LINUX_VERSION}/4.19.49/0027-ARM-stm32mp1-r2-SOUND.patch \
    file://${LINUX_VERSION}/4.19.49/0028-ARM-stm32mp1-r2-MISC.patch \
    file://${LINUX_VERSION}/4.19.49/0029-ARM-stm32mp1-r2-DEVICETREE.patch \
    file://${LINUX_VERSION}/4.19.49/0030-ARM-stm32mp1-r2-DEFCONFIG.patch \
    "

LINUX_VERSION = "4.19"

PV = "${LINUX_VERSION}"

S = "${WORKDIR}/linux-4.19.49"

# ---------------------------------
# Configure devupstream class usage
# ---------------------------------
BBCLASSEXTEND = "devupstream:target"

SRC_URI_class-devupstream = "git://github.com/STMicroelectronics/linux.git;protocol=https;branch=v${LINUX_VERSION}-stm32mp;name=linux"
SRCREV_class-devupstream = "196201973b7048ccf75aa63ac3c3673f8b6ee1c1"
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
