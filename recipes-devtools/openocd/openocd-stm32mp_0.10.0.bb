SUMMARY = "Free and Open On-Chip Debugging, In-System Programming and Boundary-Scan Testing"
HOMEPAGE = "http://openocd.org"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=b234ee4d69f5fce4486a80fdaf4a4263"

require openocd-stm32mp.inc

SRC_URI_prepend = " git://repo.or.cz/openocd.git;name=openocd "

SRCREV_FORMAT = "openocd"
SRCREV_openocd = "1afec4f561392539197fae678de4cd2ca01c127d"

PV = "0.10.0"
PR = "release.${SRCPV}"

SRC_URI += "file://0001-Add-support-of-STLINK-for-stm32mp1.patch"
SRC_URI += "file://0002-Add-support-for-silicon-revB.patch"
SRC_URI += "file://0003-Align-to-community-code-for-cache-coherency-and-rese.patch"
SRC_URI += "file://0004-Fix-init-command.patch"
SRC_URI += "file://0005-Add-CTI-plus-fixes.patch"
