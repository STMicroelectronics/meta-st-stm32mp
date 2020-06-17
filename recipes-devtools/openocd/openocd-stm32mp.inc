inherit pkgconfig autotools-brokensep gettext

SRC_URI = " \
    git://repo.or.cz/r/git2cl.git;protocol=http;destsuffix=git/tools/git2cl;name=git2cl                         \
    git://repo.or.cz/r/jimtcl.git;protocol=http;destsuffix=git/jimtcl;name=jimtcl                               \
    git://repo.or.cz/r/libjaylink.git;protocol=http;destsuffix=git/src/jtag/drivers/libjaylink;name=libjaylink  \
    "

SRCREV_git2cl = "8373c9f74993e218a08819cbcdbab3f3564bbeba"
SRCREV_jimtcl = "0aa0fb4e3a38d38a49de9eb585d93d63a370dcf6"
SRCREV_libjaylink = "f73ad5e667ae8b26a52b847c603fdadaabf302a6"

S = "${WORKDIR}/git"

BBCLASSEXTEND += "native nativesdk"

DEPENDS += "hidapi-stm32mp"
DEPENDS_class-native = "hidapi-stm32mp-native"
DEPENDS_class-nativesdk = "nativesdk-hidapi-stm32mp"

RDEPENDS_${PN} += "libusb1 hidapi-stm32mp"

EXTRA_OECONF = " \
    --disable-werror \
    --enable-stlink \
    --enable-cmsis-dap \
    MAKEINFO=true \
    HIDAPI_CFLAGS=-I${STAGING_INCDIR}/hidapi \
    HIDAPI_LIBS=-L${STAGING_LIBDIR}\ -lhidapi-libusb \
"

do_configure() {
    ./bootstrap nosubmodule
    oe_runconf ${EXTRA_OECONF}
}

do_install() {
    oe_runmake DESTDIR=${D} install
    if [ -e "${D}${infodir}" ]; then
      rm -Rf ${D}${infodir}
    fi
    if [ -e "${D}${mandir}" ]; then
      rm -Rf ${D}${mandir}
    fi
    if [ -e "${D}${bindir}/.debug" ]; then
      rm -Rf ${D}${bindir}/.debug
    fi
}

FILES_${PN} = " \
  ${datadir}/openocd/* \
  ${bindir}/openocd \
  "
