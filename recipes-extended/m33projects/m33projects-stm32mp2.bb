SUMMARY = "STM32MP2 Firmware examples for CM33"
LICENSE = "Apache-2.0 & MIT & BSD-3-Clause"
LIC_FILES_CHKSUM = "file://License.md;md5=e9544ab2a51451d422f16e04aa410c9e"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI = "git://github.com/STMicroelectronics/STM32CubeMP2.git;protocol=https;branch=main"
SRCREV  = "b9ba4e1d5b8aee76fe479e6acb1bdad600d8499a"

PV = "1.0.0"

S = "${WORKDIR}/git"

require recipes-extended/m33projects/m33projects.inc

PROJECTS_LIST = " \
    STM32MP257F-EV1/Demonstrations/USBPD_DRP_UCSI \
    STM32MP257F-DK/Demonstrations/USBPD_DRP_UCSI \
"

# WARNING: You MUST put only one project on DEFAULT_COPRO_FIRMWARE per board
# If there is several project defined for the same board while you MUST have issue at runtime
# (not the correct project could be executed).
DEFAULT_COPRO_FIRMWARE = "STM32MP257F-EV1/Demonstrations/USBPD_DRP_UCSI"
DEFAULT_COPRO_FIRMWARE += "STM32MP257F-DK/Demonstrations/USBPD_DRP_UCSI"

# Define default board reference for M33
M33_BOARDS += " STM32MP257F-EV1 STM32MP257F-DK"
