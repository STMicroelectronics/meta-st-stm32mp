LICENSE = "Apache-2.0 & MIT & BSD-3-Clause"
LIC_FILES_CHKSUM = "file://License.md;md5=012a8d78c6f636371ad889eadb15885c"

SRC_URI = "git://github.com/STMicroelectronics/STM32CubeMP2.git;protocol=https;nobranch=1"
SRCREV  = "2f7258aa45e916777ffb4f6e1b5590f65304378d"

PV = "1.3.1"


ARCHIVER_ST_BRANCH = "main"
ARCHIVER_ST_REVISION = "v${PV}"

#FIXME temporary
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = " file://CMakeLists.txt "

do_configure:prepend() {
    install -m 0644 ${WORKDIR}/CMakeLists.txt ${S}
}
