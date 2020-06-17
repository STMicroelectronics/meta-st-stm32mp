SUMMARY = "Free and Open On-Chip Debugging, In-System Programming and Boundary-Scan Testing"
HOMEPAGE = "http://openocd.org"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=b234ee4d69f5fce4486a80fdaf4a4263"

require openocd-stm32mp.inc

SRC_URI_prepend = " git://repo.or.cz/openocd.git;name=openocd "

SRCREV_FORMAT = "openocd"
SRCREV_openocd = "b5d2b1224fed3909aa3314339611ac5ac7ab0f82"

PV = "0.10.0-release.${SRCPV}"

SRC_URI += "file://0001-M4-visible-rebase-on-b5d2b1224fed-fixes.patch"
SRC_URI += "file://0002-fixes-for-gcc-10-build-macos-build-CM4-halt-stlink-J.patch"
