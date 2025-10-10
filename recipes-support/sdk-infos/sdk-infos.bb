DESCRIPTION = "information file to help to use SDK source files"
HOMEPAGE = "www.st.com"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/BSD-3-Clause;md5=550794465ba0ec5312d6919e203a55f9"

# stm32mp1
SRC_URI:stm32mp1common = " \
    file://README.HOW_TO.txt.stm32mp1;subdir=sources \
    \
    file://examples/sdk_compilation_stm32mp1_stm32mp135f-dk.example;subdir=sources \
    file://examples/sdk_compilation_stm32mp1_stm32mp157f-dk.example;subdir=sources \
    file://examples/sdk_compilation_stm32mp1_stm32mp157f-ev1.example;subdir=sources \
    "

# stm32mp2
SRC_URI:stm32mp2common = " \
    file://README.HOW_TO.txt.stm32mp2;subdir=sources \
    \
    file://examples/sdk_compilation_stm32mp2_stm32mp215f-dk.example;subdir=sources \
    file://examples/sdk_compilation_stm32mp2_stm32mp235f-dk.example;subdir=sources \
    file://examples/sdk_compilation_stm32mp2_stm32mp257f-dk.example;subdir=sources \
    file://examples/sdk_compilation_stm32mp2_stm32mp257f-ev1.example;subdir=sources \
    "

# stm32mp2 m33td
SRC_URI:stm32mp2m33tdcommon = " \
    file://README.HOW_TO.txt.stm32mp2-m33td;subdir=sources \
    \
    file://examples/sdk_compilation_stm32mp2-m33td_stm32mp215f-dk.example;subdir=sources \
    file://examples/sdk_compilation_stm32mp2-m33td_stm32mp257f-ev1.example;subdir=sources \
    "

SRC_URI:append = " \
    file://generated_build_script-stm32mpx.sh;subdir=sources/ \
    "

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(stm32mpcommon)"

inherit archiver
ARCHIVER_MODE[src] = "original"
inherit archiver_stm32mp_clean

archiver_reorder_file_for_sdk() {
    mkdir -p ${ARCHIVER_OUTDIR}/examples
    mv ${ARCHIVER_OUTDIR}/sdk_compilation* ${ARCHIVER_OUTDIR}/examples
}
do_ar_original[postfuncs] += "archiver_reorder_file_for_sdk"
