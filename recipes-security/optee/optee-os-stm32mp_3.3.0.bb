SUMMARY = "OPTEE TA development kit for stm32mp"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://LICENSE;md5=69663ab153298557a59c67a60a743e5b"

COMPATIBLE_MACHINE = "(stm32mpcommon)"

SRC_URI = "https://github.com/OP-TEE/optee_os/archive/${PV}.tar.gz"
SRC_URI[md5sum] = "7cb56c333066fd576460358fc97da85f"
SRC_URI[sha256sum] = "7b62e9fe650e197473eb2f4dc35c09d1e6395eb48dc1c16cc139d401b359ac6f"

SRC_URI += " \
    file://0001-st-updates-r1.patch \
    "

require optee-os-stm32mp.inc

PV = "3.3.0"

S = "${WORKDIR}/optee_os-${PV}"

PROVIDES += "optee-os"

do_configure_prepend(){
    chmod 755 ${S}/scripts/bin_to_c.py
}

# ---------------------------------
# Configure devupstream class usage
# ---------------------------------
BBCLASSEXTEND = "devupstream:target"

SRC_URI_class-devupstream = "git://github.com/STMicroelectronics/optee_os.git;protocol=https;branch=3.3.0-stm32mp"
SRCREV_class-devupstream = "5f5cc70dfd04419be2ba66b87f41584b6136118c"

# ---------------------------------
# Configure default preference to manage dynamic selection between tarball and github
# ---------------------------------
STM32MP_SOURCE_SELECTION ?= "tarball"

DEFAULT_PREFERENCE = "${@bb.utils.contains('STM32MP_SOURCE_SELECTION', 'github', '-1', '1', d)}"
