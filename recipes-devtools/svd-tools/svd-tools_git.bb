SUMMARY = "Python script for using CMSIS SVD parser with GDB"

LICENSE = "GPL-2.0-only"

LIC_FILES_CHKSUM = "file://LICENSE;md5=b234ee4d69f5fce4486a80fdaf4a4263"

inherit pkgconfig autotools-brokensep gettext

SRC_URI = "git://github.com/1udo6arre/svd-tools.git;protocol=https;branch=master"
SRCREV = "4faaa8e36c06f802af4d1bd45314d0f655709712"

SRC_URI += " \
    file://0001-gdb-svd-use-raw-string-to-avoid-misinterpreted-escap.patch \
    file://0002-gdb-svd-use-SVDAccessType-class.patch \
    "

BBCLASSEXTEND = "native nativesdk"

RDEPENDS:${PN} += "cmsis-svd cmsis-svd-data"
RDEPENDS:${PN} += "python3-terminaltables"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

INSTALL_PATH = "${datadir}/svd-tools"

do_install () {
    install -d ${D}${INSTALL_PATH}
    install -m 0644 ${S}/LICENSE ${D}${INSTALL_PATH}
    install -m 0755 ${S}/gdb-svd.py ${D}${INSTALL_PATH}
}

FILES:${PN} = "${INSTALL_PATH}/*"
