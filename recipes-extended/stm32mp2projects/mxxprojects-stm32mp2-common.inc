SUMMARY = "STM32MP2 Firmware examples for CMX"
LICENSE = "Apache-2.0 & MIT & BSD-3-Clause"
LIC_FILES_CHKSUM = "file://License.md;md5=012a8d78c6f636371ad889eadb15885c"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI = "git://github.com/STMicroelectronics/STM32CubeMP2.git;protocol=https;;nobranch=1"
SRCREV = "474a04c8907e466c28fae407236a8f81ea1d9e13"

SRC_URI += "file://0001-adapt-cmake_minimum_required-to-use-3.10.patch"

PV = "1.2.0"

ARCHIVER_ST_BRANCH = "main"
ARCHIVER_ST_REVISION = "v${PV}"

