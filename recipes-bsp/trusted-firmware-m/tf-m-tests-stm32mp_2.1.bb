SUMMARY = "Trusted Firmware-M Tests"
DESCRIPTION = "The Trusted Firmware-M Tests repo is meant to hold various tests for the Trusted Firmware-M software."
HOMEPAGE = "https://git.trustedfirmware.org/TF-M/tf-m-tests.git"

COMPATIBLE_MACHINE = "(stm32mp2common)"

LICENSE = "BSD-3-Clause & Apache-2.0"

LIC_FILES_CHKSUM = "file://license.rst;md5=4481bae2221b0cfca76a69fb3411f390"

SRC_URI = " git://git.trustedfirmware.org/TF-M/tf-m-tests.git;protocol=https;branch=main"

SRCREV = "6590084040144d5a768a8134fc88def2321fcb08"

SRC_URI += " \
    file://0001-v2.1.0-stm32mp-r1.patch \
    file://0002-TF-M-TEST-by-pass-git-version-check.patch\
"

TF_M_TESTS_VERSION = "v2.1.0"
TF_M_TESTS_SUBVERSION = "stm32mp"
TF_M_TESTS_RELEASE = "r1"
PV = "${TF_M_TESTS_VERSION}-${TF_M_TESTS_SUBVERSION}-${TF_M_TESTS_RELEASE}"

ARCHIVER_ST_BRANCH = "${TF_M_TESTS_VERSION}-${TF_M_TESTS_SUBVERSION}"
ARCHIVER_ST_REVISION = "${PV}"
ARCHIVER_COMMUNITY_BRANCH = "main"
ARCHIVER_COMMUNITY_REVISION = "TF-M${TF_M_TESTS_VERSION}"

include tf-m-tests-stm32mp-common.inc

# ---------------------------------
# Configure archiver use
# ---------------------------------
include ${@oe.utils.ifelse(d.getVar('ST_ARCHIVER_ENABLE') == '1', 'tf-m-tests-stm32mp-archiver.inc','')}

# ---------------------------------STAGING_EXTDT_DIR
# Configure devupstream class usage
# ---------------------------------
BBCLASSEXTEND = "devupstream:target"

SRC_URI:class-devupstream = "git://github.com/STMicroelectronics/tf-m-tests.git;protocol=https;branch=${ARCHIVER_ST_BRANCH}"
SRCREV:class-devupstream = "9c116ec247d2bac77e7b77a85109c04f3f450056"

# ---------------------------------
# Configure default preference to manage dynamic selection between tarball and github
# ---------------------------------
STM32MP_SOURCE_SELECTION ?= "tarball"

DEFAULT_PREFERENCE = "${@bb.utils.contains('STM32MP_SOURCE_SELECTION', 'github', '-1', '1', d)}"
