SUMMARY = "OPTEE TA development kit for stm32mp"
LICENSE = "BSD-2-Clause & BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE;md5=c1f21c4f72f372ef38a5a4aee55ec173"

SRC_URI = "git://github.com/OP-TEE/optee_os.git;protocol=https"
SRCREV = "af141c61fe7a2430f3b4bb89661d8414117013b3"

SRC_URI += " \
    file://0001-3.9.0-stm32mp-r1.patch \
    file://0002-3.9.0-stm32mp-r2.patch \
    file://0003-3.9.0-stm32mp-r2.2.patch \
    file://0004-3.9.0-stm32mp-r2.3.patch \
    file://0005-3.9.0-stm32mp-r2.4.patch \
    "

OPTEE_VERSION = "3.9.0"
PV = "${OPTEE_VERSION}.r2.4"

S = "${WORKDIR}/git"

COMPATIBLE_MACHINE = "(stm32mpcommon)"

OPTEEMACHINE ?= "stm32mp1"
OPTEEMACHINE_stm32mp1common = "stm32mp1"

OPTEEOUTPUTMACHINE ?= "stm32mp1"
OPTEEOUTPUTMACHINE_stm32mp1common = "stm32mp1"


# The package is empty but must be generated to avoid apt-get installation issue
ALLOW_EMPTY_${PN} = "1"

require optee-os-stm32mp-common.inc

# ---------------------------------
# Configure archiver use
# ---------------------------------
include ${@oe.utils.ifelse(d.getVar('ST_ARCHIVER_ENABLE') == '1', 'optee-os-stm32mp-archiver.inc','')}

# ---------------------------------
# Configure devupstream class usage
# ---------------------------------
BBCLASSEXTEND = "devupstream:target"

SRC_URI_class-devupstream = "git://github.com/STMicroelectronics/optee_os.git;protocol=https;branch=${OPTEE_VERSION}-stm32mp"
SRCREV_class-devupstream = "3e96d660560b40d5fc549da09ce1c5fc4472267a"

# ---------------------------------
# Configure default preference to manage dynamic selection between tarball and github
# ---------------------------------
STM32MP_SOURCE_SELECTION ?= "tarball"

DEFAULT_PREFERENCE = "${@bb.utils.contains('STM32MP_SOURCE_SELECTION', 'github', '-1', '1', d)}"
