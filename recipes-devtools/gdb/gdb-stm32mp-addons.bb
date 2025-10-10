SUMMARY = "STM32MP GDB init file"
HOMEPAGE = "http://www.st.com"
DESCRIPTION = "STM32MP GDB init file"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/BSD-3-Clause;md5=550794465ba0ec5312d6919e203a55f9"

#Add scripts for gdb/openocd/eclipse
SRC_URI = " \
    file://gdbinit \
"

S = "${UNPACKDIR}"

do_install() {
   install -d ${D}/${bindir}/
   install -m 0644 ${UNPACKDIR}/gdbinit ${D}/${bindir}/
}
BBCLASSEXTEND += "native nativesdk"
