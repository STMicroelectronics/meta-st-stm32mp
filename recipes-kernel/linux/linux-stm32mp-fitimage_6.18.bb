SUMMARY = "Linux STM32MP Kernel for fit image"
SECTION = "kernel"

LICENSE = "GPL-2.0-with-Linux-syscall-note"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0-with-Linux-syscall-note;md5=0bad96c422c41c3a94009dcfe1bff992"

include recipes-bsp/u-boot/u-boot-stm32mp-config.inc
inherit linux-kernel-base kernel-fit-image

# Set the version of this recipe to the version of the included kernel
# (without taking the long way around via PV)
PKGV = "${@get_kernelversion_file("${STAGING_KERNEL_BUILDDIR}")}"

KERNEL_DEPLOYSUBDIR ?= "kernel"
