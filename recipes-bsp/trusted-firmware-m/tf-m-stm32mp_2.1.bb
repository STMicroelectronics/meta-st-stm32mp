SUMMARY = "Trusted Firmware for Cortex-M"
DESCRIPTION = "Trusted Firmware-M"
HOMEPAGE = "https://git.trustedfirmware.org/TF-M/trusted-firmware-m.git"
PROVIDES = "virtual/trusted-firmware-m"
PROVIDES:append::virtclass-multilib-lib64 = " virtual/trusted-firmware-m "

COMPATIBLE_MACHINE = "(stm32mp2common)"

LICENSE = "BSD-3-Clause & Apache-2.0"

LIC_FILES_CHKSUM = "file://license.rst;md5=07f368487da347f3c7bd0fc3085f3afa"

SRC_URI = "git://git.trustedfirmware.org/TF-M/trusted-firmware-m.git;protocol=https;nobranch=1;name=tfm"
SRC_URI:append = " gitsm://github.com/Mbed-TLS/mbedtls.git;protocol=https;branch=main;name=mbedtls;destsuffix=${TFM_EXTERNAL_SOURCES_ROOTDIR}/${TF_M_PATH_MBEDCRYPTO}"
SRC_URI:append = " gitsm://github.com/mcu-tools/mcuboot.git;protocol=https;branch=main;name=mcuboot;destsuffix=${TFM_EXTERNAL_SOURCES_ROOTDIR}/${TF_M_PATH_MCUBOOT}"
SRC_URI:append = " git://github.com/laurencelundblade/QCBOR.git;protocol=https;branch=master;name=qcbor;destsuffix=${TFM_EXTERNAL_SOURCES_ROOTDIR}/${TF_M_PATH_QCBOR}"
SRC_URI:append = " git://github.com/ARM-software/CMSIS_6.git;protocol=https;nobranch=1;name=cmsis;destsuffix=${TFM_EXTERNAL_SOURCES_ROOTDIR}/${TF_M_PATH_CMSIS}"
SRC_URI:append = " git://github.com/STMicroelectronics/stm32-ddr-phy-binary;protocol=https;branch=main;name=ddr-phy;destsuffix=${TFM_EXTERNAL_SOURCES_ROOTDIR}/${TF_M_PATH_DDR_PHY_BIN_SRC}"
SRC_URI:append = " gitsm://github.com/STMicroelectronics/SCP-firmware.git;protocol=https;name=scp-firmware;destsuffix=${TFM_EXTERNAL_SOURCES_ROOTDIR}/${TF_M_PATH_SCP_FW};branch=v2.13.0-stm32mp"

# The required dependencies are documented in tf-m/config/config_base.cmake
# TF-Mv 2.1.0
SRCREV_tfm = "0c4c99ba33b3e66deea070e149279278dc7647f4"

TF_M_PATH_EXTERNAL = "external"
# mbedtls
TF_M_PATH_MBEDCRYPTO = "external/mbedtls"
ARCHIVER_REVISION_MBEDCRYPTO = "v3.6.0"
SRCREV_mbedtls = "2ca6c285a0dd3f33982dd57299012dacab1ff206"
# mcuboot
TF_M_PATH_MCUBOOT = "external/mcuboot"
ARCHIVER_REVISION_MCUBOOT = "v2.1.0"
SRCREV_mcuboot = "9c99326b9756dbcc35b524636d99ed5f3e6cb29b"
# qcbor
TF_M_PATH_QCBOR = "external/qcbor"
ARCHIVER_REVISION_QCBOR = "v1.2"
SRCREV_qcbor = "92d3f89030baff4af7be8396c563e6c8ef263622"
# cmsis v6.0.0
TF_M_PATH_CMSIS = "external/cmsis"
# Select v6.0.0 version plus few commits (no associated tag)
#ARCHIVER_REVISION_CMSIS = "v6.0.0"
ARCHIVER_REVISION_CMSIS = "d0c460c1697d210b49a4b90998195831c0cd325c"
SRCREV_cmsis = "d0c460c1697d210b49a4b90998195831c0cd325c"
# ddr-phy
TF_M_PATH_DDR_PHY_BIN_SRC = "external/stm32-ddr-phy-binary"
ARCHIVER_REVISION_DDR_PHY_BIN_SRC = "v1.0-stm32mp"
SRCREV_ddr-phy = "77447cf214eadf128e487fcb10a4a78cd4ab6d56"
# scp-firmware
TF_M_PATH_SCP_FW = "external/scp-firmware"
ARCHIVER_REVISION_SCP_FW = "v2.13.0-stm32mp-r2"
SRCREV_scp-firmware = "f70a89c8378429c65184d816ea7ebbe58df67fee"

# In order to avoid issue with devtool and gitsm, we use dedicated folder not
# included inside tf-m source code.
# For archiver class, in order to extract all source code in a row, we configure
# tf-m extrenal source inside tf-m git source code
TFM_EXTERNAL_SOURCES_ROOTDIR = "${@'git' if bb.data.inherits_class('archiver', d) and not bb.data.inherits_class('devtool-source', d) else 'tf-m-external-src'}"

SRCREV_FORMAT = "tfm"

UPSTREAM_CHECK_GITTAGREGEX = "^TF-Mv(?P<pver>\d+(\.\d+)+)$"

# ST Patches
SRC_URI += " \
    file://0001-v2.1.0-stm32mp-r1.patch.gz \
"

TF_M_VERSION = "2.1.0"
TF_M_SUBVERSION = "stm32mp"
TF_M_RELEASE = "r1"
PV = "v${TF_M_VERSION}-${TF_M_SUBVERSION}-${TF_M_RELEASE}"

ARCHIVER_ST_BRANCH = "v${TF_M_VERSION}-${TF_M_SUBVERSION}"
ARCHIVER_ST_REVISION = "v${TF_M_VERSION}-${TF_M_SUBVERSION}-${TF_M_RELEASE}"
ARCHIVER_COMMUNITY_BRANCH = "v${TF_M_VERSION}"
ARCHIVER_COMMUNITY_REVISION = "v${TF_M_VERSION}"

include tf-m-stm32mp-common.inc

# ---------------------------------
# Configure archiver use
# ---------------------------------
include ${@oe.utils.ifelse(d.getVar('ST_ARCHIVER_ENABLE') == '1', 'tf-m-stm32mp-archiver.inc','')}

# ---------------------------------
# Configure devupstream class usage
# ---------------------------------
BBCLASSEXTEND = "devupstream:target"

SRC_URI:class-devupstream = "git://github.com/STMicroelectronics/trusted-firmware-m.git;protocol=https;branch=${ARCHIVER_ST_BRANCH}"
SRCREV:class-devupstream = "e1278028d2c5a6d1dc85a47d0cff86f180a616f9"


# ---------------------------------
# Configure default preference to manage dynamic selection between tarball and github
# ---------------------------------
STM32MP_SOURCE_SELECTION ?= "tarball"

DEFAULT_PREFERENCE = "${@bb.utils.contains('STM32MP_SOURCE_SELECTION', 'github', '-1', '1', d)}"
