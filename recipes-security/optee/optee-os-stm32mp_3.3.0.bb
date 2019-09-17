SUMMARY = "OPTEE TA development kit for stm32mp"
LICENSE = "BSD-2-Clause & BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE;md5=69663ab153298557a59c67a60a743e5b"

SRC_URI = "https://github.com/OP-TEE/optee_os/archive/${PV}.tar.gz"
SRC_URI[md5sum] = "7cb56c333066fd576460358fc97da85f"
SRC_URI[sha256sum] = "7b62e9fe650e197473eb2f4dc35c09d1e6395eb48dc1c16cc139d401b359ac6f"

SRC_URI += " \
    file://0001-st-updates-r1.patch \
    file://0002-st-updates-r2.patch \
   "

OPTEE_VERSION = "3.3.0"
PV = "${OPTEE_VERSION}"

S = "${WORKDIR}/optee_os-${PV}"

COMPATIBLE_MACHINE = "(stm32mpcommon)"

PROVIDES += "optee-os"

require optee-os-stm32mp-common.inc

# ---------------------------------
# Configure archiver use
# ---------------------------------
include ${@oe.utils.ifelse(d.getVar('ST_ARCHIVER_ENABLE') == '1', 'optee-os-stm32mp-archiver.inc','')}

# ---------------------------------
# Configure devupstream class usage
# ---------------------------------
BBCLASSEXTEND = "devupstream:target"

SRC_URI_class-devupstream = "git://github.com/STMicroelectronics/optee_os.git;protocol=https;name=opteeos;branch=3.3.0-stm32mp"
SRCREV_class-devupstream = "5f5cc70dfd04419be2ba66b87f41584b6136118c"
SRCREV_FORMAT_class-devupstream = "opteeos"
PV_class-devupstream = "${OPTEE_VERSION}+github+${SRCPV}"

# ---------------------------------
# Configure default preference to manage dynamic selection between tarball and github
# ---------------------------------
STM32MP_SOURCE_SELECTION ?= "tarball"

DEFAULT_PREFERENCE = "${@bb.utils.contains('STM32MP_SOURCE_SELECTION', 'github', '-1', '1', d)}"
