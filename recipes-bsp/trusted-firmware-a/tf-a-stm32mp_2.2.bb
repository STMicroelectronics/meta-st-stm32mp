require tf-a-stm32mp-common_${PV}.inc
require tf-a-stm32mp-common.inc

SUMMARY = "Trusted Firmware-A for STM32MP1"
LICENSE = "BSD-3-Clause"

PROVIDES += "virtual/trusted-firmware-a"

# Configure stm32mp1 make settings
EXTRA_OEMAKE += 'PLAT=stm32mp1'
EXTRA_OEMAKE += 'ARCH=aarch32'
EXTRA_OEMAKE += 'ARM_ARCH_MAJOR=7'
# Configure default mode (All supported device type)
EXTRA_OEMAKE += 'STM32MP_SDMMC=1'
EXTRA_OEMAKE += 'STM32MP_EMMC=1'
EXTRA_OEMAKE += 'STM32MP_SPI_NOR=1'
EXTRA_OEMAKE += 'STM32MP_RAW_NAND=1'
EXTRA_OEMAKE += 'STM32MP_SPI_NAND=1'

# Enable the wrapper for debug
TF_A_ENABLE_DEBUG_WRAPPER ?= "1"

# ---------------------------------
# Configure archiver use
# ---------------------------------
include ${@oe.utils.ifelse(d.getVar('ST_ARCHIVER_ENABLE') == '1', 'tf-a-stm32mp-archiver.inc','')}

# ---------------------------------
# Configure devupstream class usage
# ---------------------------------
BBCLASSEXTEND = "devupstream:target"

SRC_URI_class-devupstream = "git://github.com/STMicroelectronics/arm-trusted-firmware.git;protocol=https;branch=v${TF_VERSION}-stm32mp"
SRCREV_class-devupstream = "e587179e7b0823b5c6e38e029e417d10f088db51"

# ---------------------------------
# Configure default preference to manage dynamic selection between tarball and github
# ---------------------------------
STM32MP_SOURCE_SELECTION ?= "tarball"

DEFAULT_PREFERENCE = "${@bb.utils.contains('STM32MP_SOURCE_SELECTION', 'github', '-1', '1', d)}"
