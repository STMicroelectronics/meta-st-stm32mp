SUMMARY = "Linux STM32MP Kernel"
SECTION = "kernel"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"

include linux-stm32mp.inc

SRC_URI = "https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.19.9.tar.xz"
SRC_URI[md5sum] = "d7e09d6be85ec8548c73e8713531e958"
SRC_URI[sha256sum] = "fc116cc6829c73944215d3b3ac0fc368dde9e8235b456744afffde001269dbf2"

SRC_URI += " \
    file://${LINUX_VERSION}/4.19.9/0001-ARM-stm32mp1-r0-rc1-MACHINE.patch \
    file://${LINUX_VERSION}/4.19.9/0002-ARM-stm32mp1-r0-rc1-CLOCK.patch \
    file://${LINUX_VERSION}/4.19.9/0003-ARM-stm32mp1-r0-rc1-DMA.patch \
    file://${LINUX_VERSION}/4.19.9/0004-ARM-stm32mp1-r0-rc1-I2C.patch \
    file://${LINUX_VERSION}/4.19.9/0005-ARM-stm32mp1-r0-rc1-IIO.patch \
    file://${LINUX_VERSION}/4.19.9/0006-ARM-stm32mp1-r0-rc1-IRQ-Mailbox.patch \
    file://${LINUX_VERSION}/4.19.9/0007-ARM-stm32mp1-r0-rc1-INPUT-TTY.patch \
    file://${LINUX_VERSION}/4.19.9/0008-ARM-stm32mp1-r0-rc1-MFD.patch \
    file://${LINUX_VERSION}/4.19.9/0009-ARM-stm32mp1-r0-rc1-MMC-MTD.patch \
    file://${LINUX_VERSION}/4.19.9/0010-ARM-stm32mp1-r0-rc1-ETH.patch \
    file://${LINUX_VERSION}/4.19.9/0011-ARM-stm32mp1-r0-rc1-NVMEM.patch \
    file://${LINUX_VERSION}/4.19.9/0012-ARM-stm32mp1-r0-rc1-PINCTRL-PWM-RESET-RTC.patch \
    file://${LINUX_VERSION}/4.19.9/0013-ARM-stm32mp1-r0-rc1-REMOTEPROC-RPMSG.patch \
    file://${LINUX_VERSION}/4.19.9/0014-ARM-stm32mp1-r0-rc1-WATCHDOG.patch \
    file://${LINUX_VERSION}/4.19.9/0015-ARM-stm32mp1-r0-rc1-MISC.patch \
    file://${LINUX_VERSION}/4.19.9/0016-ARM-stm32mp1-r0-rc1-DEVICETREE.patch \
    file://${LINUX_VERSION}/4.19.9/0017-ARM-stm32mp1-r0-rc1-DEFCONFIG.patch \
    file://${LINUX_VERSION}/4.19.9/0018-ARM-stm32mp1-r0-rc2-DRM-KMS.patch \
    file://${LINUX_VERSION}/4.19.9/0019-ARM-stm32mp1-r0-rc2-SOUND.patch \
    file://${LINUX_VERSION}/4.19.9/0020-ARM-stm32mp1-r0-rc2-MEDIA.patch \
    file://${LINUX_VERSION}/4.19.9/0021-ARM-stm32mp1-r0-rc2-PINCTRL.patch \
    file://${LINUX_VERSION}/4.19.9/0022-ARM-stm32mp1-r0-rc2-MFD-IRQ.patch \
    file://${LINUX_VERSION}/4.19.9/0023-ARM-stm32mp1-r0-rc2-USB.patch \
    file://${LINUX_VERSION}/4.19.9/0024-ARM-stm32mp1-r0-rc2-THERMAL.patch \
    file://${LINUX_VERSION}/4.19.9/0025-ARM-stm32mp1-r0-rc2-REMOTEPROC.patch \
    file://${LINUX_VERSION}/4.19.9/0026-ARM-stm32mp1-r0-rc2-NET.patch \
    file://${LINUX_VERSION}/4.19.9/0027-ARM-stm32mp1-r0-rc2-HWCLK-SPI.patch \
    file://${LINUX_VERSION}/4.19.9/0028-ARM-stm32mp1-r0-rc2-MMC.patch \
    file://${LINUX_VERSION}/4.19.9/0029-ARM-stm32mp1-r0-rc2-HWSPINLOCK-IIO-I2C.patch \
    file://${LINUX_VERSION}/4.19.9/0030-ARM-stm32mp1-r0-rc2-DEVICETREE.patch \
    file://${LINUX_VERSION}/4.19.9/0031-ARM-stm32mp1-r0-rc2-DEFCONFIG.patch \
    file://${LINUX_VERSION}/4.19.9/0032-ARM-stm32mp1-r0-rc3-DMA.patch \
    file://${LINUX_VERSION}/4.19.9/0033-ARM-stm32mp1-r0-rc3-DISPLAY.patch \
    file://${LINUX_VERSION}/4.19.9/0034-ARM-stm32mp1-r0-rc3-ETH.patch \
    file://${LINUX_VERSION}/4.19.9/0035-ARM-stm32mp1-r0-rc3-IIO.patch \
    file://${LINUX_VERSION}/4.19.9/0036-ARM-stm32mp1-r0-rc3-INPUT-TTY.patch \
    file://${LINUX_VERSION}/4.19.9/0037-ARM-stm32mp1-r0-rc3-IRQ-Mailbox.patch \
    file://${LINUX_VERSION}/4.19.9/0038-ARM-stm32mp1-r0-rc3-MEDIA.patch \
    file://${LINUX_VERSION}/4.19.9/0039-ARM-stm32mp1-r0-rc3-MMC-MTD.patch \
    file://${LINUX_VERSION}/4.19.9/0040-ARM-stm32mp1-r0-rc3-PINCTRL-PWM-RESET-RTC.patch \
    file://${LINUX_VERSION}/4.19.9/0041-ARM-stm32mp1-r0-rc3-REMOTEPROC-RPMSG.patch \
    file://${LINUX_VERSION}/4.19.9/0042-ARM-stm32mp1-r0-rc3-SOUND.patch \
    file://${LINUX_VERSION}/4.19.9/0043-ARM-stm32mp1-r0-rc3-MISC.patch \
    file://${LINUX_VERSION}/4.19.9/0044-ARM-stm32mp1-r0-rc3-DEVICETREE.patch \
    file://${LINUX_VERSION}/4.19.9/0045-ARM-stm32mp1-r0-rc3-DEFCONFIG.patch \
    file://${LINUX_VERSION}/4.19.9/0046-ARM-stm32mp1-r0-rc4-MMC-MTD.patch \
    file://${LINUX_VERSION}/4.19.9/0047-ARM-stm32mp1-r0-rc4-PINCTRL-PWM-RESET-RTC.patch \
    file://${LINUX_VERSION}/4.19.9/0048-ARM-stm32mp1-r0-rc4-REMOTEPROC-RPMSG.patch \
    file://${LINUX_VERSION}/4.19.9/0049-ARM-stm32mp1-r0-rc4-SOUND.patch \
    file://${LINUX_VERSION}/4.19.9/0050-ARM-stm32mp1-r0-rc4-USB.patch \
    file://${LINUX_VERSION}/4.19.9/0051-ARM-stm32mp1-r0-rc4-DEVICETREE.patch \
    file://${LINUX_VERSION}/4.19.9/0052-ARM-stm32mp1-r0-rc4-DEFCONFIG.patch \
    \
    file://${LINUX_VERSION}/4.19.9/0053-ARM-stm32mp1-r0-rc4-hotfix-w903.1-DRIVERS.patch \
    file://${LINUX_VERSION}/4.19.9/0054-ARM-stm32mp1-r0-rc4-hotfix-w903.1-DEVICETREE.patch \
    file://${LINUX_VERSION}/4.19.9/0055-ARM-stm32mp1-r0-rc4-hotfix-w903.1-DEFCONFIG.patch \
    \
    file://${LINUX_VERSION}/4.19.9/0056-ARM-stm32mp1-r0-rc4-hotfix-w903.3-DRIVERS.patch \
    file://${LINUX_VERSION}/4.19.9/0057-ARM-stm32mp1-r0-rc4-hotfix-w903.3-DEVICETREE.patch \
    file://${LINUX_VERSION}/4.19.9/0058-ARM-stm32mp1-r0-rc4-hotfix-w903.3-DEFCONFIG.patch \
    \
    file://${LINUX_VERSION}/4.19.9/0059-ARM-stm32mp1-r0-rc4-hotfix-w904.3-DRIVERS.patch \
    file://${LINUX_VERSION}/4.19.9/0060-ARM-stm32mp1-r0-rc4-hotfix-w904.3-DEVICETREE.patch \
    \
    file://${LINUX_VERSION}/4.19.9/0061-ARM-stm32mp1-r0-rc4-hotfix-w904.5-DRIVERS.patch \
    file://${LINUX_VERSION}/4.19.9/0062-ARM-stm32mp1-r0-rc4-hotfix-w904.5-DEVICETREE.patch \
    \
    file://${LINUX_VERSION}/4.19.9/0063-ARM-stm32mp1-r0-rc4-hotfix-w905.2-DRIVERS.patch \
    file://${LINUX_VERSION}/4.19.9/0064-ARM-stm32mp1-r0-rc4-hotfix-w905.2-DEVICETREE.patch \
    "

LINUX_VERSION = "4.19"

PV = "${LINUX_VERSION}"

S = "${WORKDIR}/linux-4.19.9"

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

# -------------------------------------------------------------
# Archiver
#
inherit archiver
ARCHIVER_MODE[src] = "${@'original' if d.getVar('ST_ARCHIVER_ENABLE') == '1' else ''}"
SRC_URI =+ "file://README.HOW_TO.txt"

inherit archiver_stm32mp_clean
