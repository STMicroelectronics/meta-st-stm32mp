SUMMARY = "OPTEE TA development kit for stm32mp"
LICENSE = "BSD-2-Clause & BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE;md5=c1f21c4f72f372ef38a5a4aee55ec173"

SRC_URI = "git://github.com/OP-TEE/optee_os.git;protocol=https;name=os"
SRCREV = "3d47a131bca1d9ed511bfd516aa5e70269e12c1d"

SRC_URI += " \
    file://0001-3.12.0-stm32mp-r1.patch \
    file://0002-3.12.0-stm32mp-r1.1-rc1.patch \
    file://0003-3.12.0-stm32mp-r2.patch \
    "

OPTEE_VERSION = "3.12.0"
OPTEE_SUBVERSION = "stm32mp"
OPTEE_RELEASE = "r2"

PV = "${OPTEE_VERSION}-${OPTEE_SUBVERSION}-${OPTEE_RELEASE}"

ARCHIVER_ST_BRANCH = "${OPTEE_VERSION}-${OPTEE_SUBVERSION}"
ARCHIVER_ST_REVISION = "${PV}"
ARCHIVER_COMMUNITY_BRANCH = "master"
ARCHIVER_COMMUNITY_REVISION = "${OPTEE_VERSION}"

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

SRC_URI_class-devupstream = "git://github.com/STMicroelectronics/optee_os.git;protocol=https;branch=${ARCHIVER_ST_BRANCH}"
SRCREV_class-devupstream = "639a8566de8fa720d2cb7ab7231e8de105e7859d"

# ---------------------------------
# Configure default preference to manage dynamic selection between tarball and github
# ---------------------------------
STM32MP_SOURCE_SELECTION ?= "tarball"

DEFAULT_PREFERENCE = "${@bb.utils.contains('STM32MP_SOURCE_SELECTION', 'github', '-1', '1', d)}"
