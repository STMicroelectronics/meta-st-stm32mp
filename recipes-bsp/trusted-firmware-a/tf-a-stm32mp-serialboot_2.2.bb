require tf-a-stm32mp-common_${PV}.inc
require tf-a-stm32mp-common.inc

SUMMARY = "Trusted Firmware-A for STM32MP1 as serial boot loader"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://license.rst;md5=1dd070c98a281d18d9eefd938729b031"

PROVIDES += "virtual/trusted-firmware-a-serialboot"

TFA_SHARED_SOURCES = "0"

# Configure stm32mp1 make settings
EXTRA_OEMAKE += 'PLAT=stm32mp1'
EXTRA_OEMAKE += 'ARCH=aarch32'
EXTRA_OEMAKE += 'ARM_ARCH_MAJOR=7'
# Configure all serial boot supports
EXTRA_OEMAKE += 'STM32MP_UART_PROGRAMMER=1'
EXTRA_OEMAKE += 'STM32MP_USB_PROGRAMMER=1'

# Disable the wrapper for debug
TF_A_ENABLE_DEBUG_WRAPPER ?= "0"
