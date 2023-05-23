FILESEXTRAPATHS:prepend := "${THISDIR}/tf-a-stm32mp:"

SECTION = "bootloaders"

LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://license.rst;md5=1dd070c98a281d18d9eefd938729b031"
CVE_PRODUCT = "arm:trusted_firmware-a"

SRC_URI = "git://git.trustedfirmware.org/TF-A/trusted-firmware-a.git;protocol=https;branch=lts-${TF_A_VERSION}"

SRCREV = "ff0bd5f9bb2ba2f31fb9cec96df917747af9e92d"

SRC_URI += " \
    file://0001-${TF_A_VERSION}-${TF_A_TARGET}-${TF_A_RELEASE}.patch \
    "

TF_A_VERSION = "v2.8"
TF_A_SUBVERSION = ".6"
TF_A_TARGET = "stm32mp"
TF_A_RELEASE = "r1"
PV = "${TF_A_VERSION}${TF_A_SUBVERSION}-${TF_A_TARGET}-${TF_A_RELEASE}"

ARCHIVER_ST_BRANCH = "${TF_A_VERSION}-${TF_A_TARGET}"
ARCHIVER_ST_REVISION = "${TF_A_VERSION}-${TF_A_TARGET}-${TF_A_RELEASE}"
ARCHIVER_COMMUNITY_BRANCH = "lts-${TF_A_VERSION}"
ARCHIVER_COMMUNITY_REVISION = "lts-${TF_A_VERSION}${TF_A_SUBVERSION}"

S = "${WORKDIR}/git"

# ---------------------------------
# Configure devupstream class usage
# ---------------------------------
BBCLASSEXTEND = "devupstream:target"

SRC_URI:class-devupstream = "git://github.com/STMicroelectronics/arm-trusted-firmware.git;protocol=https;branch=${ARCHIVER_ST_BRANCH}"
SRCREV:class-devupstream = "230b4d8447a1b07bb92035e19ccec8c633103a08"

# ---------------------------------
# Configure default preference to manage dynamic selection between tarball and github
# ---------------------------------
STM32MP_SOURCE_SELECTION ?= "tarball"

DEFAULT_PREFERENCE = "${@bb.utils.contains('STM32MP_SOURCE_SELECTION', 'github', '-1', '1', d)}"
