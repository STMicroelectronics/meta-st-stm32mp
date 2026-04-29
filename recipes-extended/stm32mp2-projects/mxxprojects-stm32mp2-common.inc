LICENSE = "Apache-2.0 & MIT & BSD-3-Clause"
LIC_FILES_CHKSUM = "file://License.md;md5=f2f110de9c50e738bfd1e15ef52181c4"

SRC_URI = "git://github.com/STMicroelectronics/STM32CubeMP2.git;protocol=https;nobranch=1"
SRCREV  = "38732d24f8928af2dbcd9491f92390efbaec78d6"

PV = "1.3.0"

SRC_URI += "file://0001-adapt-cmake_minimum_required-to-use-3.10.patch"

ARCHIVER_ST_BRANCH = "main"
ARCHIVER_ST_REVISION = "v${PV}"

#FIXME temporary
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = " file://CMakeLists.txt "

do_configure:prepend() {
    install -m 0644 ${UNPACKDIR}/CMakeLists.txt ${S}
}
