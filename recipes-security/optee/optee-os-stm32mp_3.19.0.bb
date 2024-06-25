SUMMARY = "OPTEE TA development kit for stm32mp"
LICENSE = "BSD-2-Clause & BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE;md5=c1f21c4f72f372ef38a5a4aee55ec173"

SRC_URI = "git://github.com/OP-TEE/optee_os.git;protocol=https;branch=master"
SRCREV = "afacf356f9593a7f83cae9f96026824ec242ff52"

SRC_URI += " \
    file://fonts.tar.gz;subdir=git;name=fonts  \
    file://0001-3.19.0-stm32mp-r2.patch \
    "

SRC_URI[fonts.sha256sum] = "4941e8bb6d8ac377838e27b214bf43008c496a24a8f897e0b06433988cbd53b2"

OPTEE_VERSION = "3.19.0"
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
OPTEEMACHINE:stm32mp1common = "stm32mp1"
OPTEEMACHINE:stm32mp2common = "stm32mp2"

OPTEEOUTPUTMACHINE ?= "stm32mp1"
OPTEEOUTPUTMACHINE:stm32mp1common = "stm32mp1"
OPTEEOUTPUTMACHINE:stm32mp2common = "stm32mp2"

# The package is empty but must be generated to avoid apt-get installation issue
ALLOW_EMPTY:${PN} = "1"

require optee-os-stm32mp-common.inc

# ---------------------------------
# Configure archiver use
# ---------------------------------
include ${@oe.utils.ifelse(d.getVar('ST_ARCHIVER_ENABLE') == '1', 'optee-os-stm32mp-archiver.inc','')}

# ---------------------------------
# Configure devupstream class usage
# ---------------------------------
BBCLASSEXTEND = "devupstream:target"

SRC_URI:class-devupstream = "git://github.com/STMicroelectronics/optee_os.git;protocol=https;branch=${ARCHIVER_ST_BRANCH}"
SRCREV:class-devupstream = "43089782f25c57968bea4e2794fb82101d31227c"

# ---------------------------------
# Configure default preference to manage dynamic selection between tarball and github
# ---------------------------------
STM32MP_SOURCE_SELECTION ?= "tarball"

DEFAULT_PREFERENCE = "${@bb.utils.contains('STM32MP_SOURCE_SELECTION', 'github', '-1', '1', d)}"
