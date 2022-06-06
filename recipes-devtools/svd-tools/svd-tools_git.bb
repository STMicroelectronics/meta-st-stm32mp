SUMMARY = "Python script for using CMSIS SVD parser with GDB"

LICENSE = "GPL-2.0-only"

LIC_FILES_CHKSUM = "file://LICENSE;md5=b234ee4d69f5fce4486a80fdaf4a4263"

inherit pkgconfig autotools-brokensep gettext

SRC_URI = "git://github.com/1udo6arre/svd-tools.git;protocol=https;branch=master"
SRCREV = "5b7b813481877a3b6fb8f96b4f0d413b47fb987a"

S = "${WORKDIR}/git"

BBCLASSEXTEND = "native nativesdk"

RDEPENDS:${PN} += "cmsis-svd"
RRECOMMENDS:${PN}:append:class-nativesdk = " nativesdk-cmsis-svd "
# For python dependencies
RDEPENDS:${PN} += "python3-terminaltables"
RRECOMMENDS:${PN}:append:class-nativesdk = " nativesdk-python3-terminaltables "

do_configure[noexec] = "1"
do_compile[noexec] = "1"

INSTALL_PATH= "${datadir}/svd-tools"

do_install () {
    install -d ${D}${INSTALL_PATH}
    install -m 0644 ${S}/LICENSE ${D}${INSTALL_PATH}
    install -m 0755 ${S}/gdb-svd.py ${D}${INSTALL_PATH}
}

FILES:${PN} = "${INSTALL_PATH}/*"
