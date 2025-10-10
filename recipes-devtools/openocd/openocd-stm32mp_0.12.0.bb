SUMMARY = "Free and Open On-Chip Debugging, In-System Programming and Boundary-Scan Testing"
HOMEPAGE = "http://openocd.org"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=599d2d1ee7fc84c0467b3d19801db870"

require openocd-stm32mp.inc

SRC_URI = "git://github.com/openocd-org/openocd.git;protocol=https;branch=master;name=openocd "

SRCREV_FORMAT = "openocd"
SRCREV_openocd = "6554d176e926e1e46b90e1b00d1b3ed1bd20b9ff"

PV = "0.12.0+dev.${SRCPV}-stm32mp-r2.1"

SRC_URI += " \
    file://0001-v0.12.0-stm32mp-r2.1.patch \
    "
