SUMMARY = "Free and Open On-Chip Debugging, In-System Programming and Boundary-Scan Testing"
HOMEPAGE = "http://openocd.org"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=b234ee4d69f5fce4486a80fdaf4a4263"

require openocd-stm32mp.inc

SRC_URI_prepend = "git://repo.or.cz/openocd.git;name=openocd "

SRCREV_FORMAT = "openocd"
SRCREV_openocd = "a5e526d8575cf63fe11babec85c0798ac3f4ad74"

PV = "0.11.0-rc2.${SRCPV}"

SRC_URI += " \
    file://0001-rebase-on-v0.11.0-rc2.patch \
"
