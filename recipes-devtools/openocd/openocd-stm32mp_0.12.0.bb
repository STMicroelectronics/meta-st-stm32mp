SUMMARY = "Free and Open On-Chip Debugging, In-System Programming and Boundary-Scan Testing"
HOMEPAGE = "http://openocd.org"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=599d2d1ee7fc84c0467b3d19801db870"

require openocd-stm32mp.inc

SRC_URI = "git://github.com/openocd-org/openocd.git;protocol=https;branch=master;name=openocd "

SRCREV_FORMAT = "openocd"
SRCREV_openocd = "2c8376b79d104a855bd3a559e59edf330309bcad"

PV = "0.12.0+dev.${SRCPV}-stm32mp25-r1-rc2"

SRC_URI += " \
    file://0001-v0.12.0-stm32mp-r1.patch \
    "

# Use jimtcl master branch to fix RANLIB issue in kirkstone and commit it
# to prevent "-dirty" suffix to openocd version.
# To be removed after a new jimtcl release get used by openocd.
do_configure:prepend() {
	git add jimtcl
	git commit --allow-empty -m "Update jimtcl"
}
