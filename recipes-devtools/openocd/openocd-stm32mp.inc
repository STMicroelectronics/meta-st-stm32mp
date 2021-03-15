DEPENDS = "hidapi-stm32mp"

RDEPENDS_${PN} += "libusb1 hidapi-stm32mp"

inherit pkgconfig autotools-brokensep gettext

SRC_URI = " \
    git://repo.or.cz/git2cl.git;destsuffix=git/tools/git2cl;name=git2cl                         \
    git://github.com/msteveb/jimtcl;protocol=https;destsuffix=git/jimtcl;name=jimtcl            \
    git://repo.or.cz/libjaylink.git;destsuffix=git/src/jtag/drivers/libjaylink;name=libjaylink  \
    "

SRCREV_git2cl = "8373c9f74993e218a08819cbcdbab3f3564bbeba"
SRCREV_jimtcl = "0aa0fb4e3a38d38a49de9eb585d93d63a370dcf6"
SRCREV_libjaylink = "9aa7a5957c07bb6e862fc1a6d3153d109c7407e4"

S = "${WORKDIR}/git"

BBCLASSEXTEND += "native nativesdk"

DEPENDS += "hidapi-stm32mp"
DEPENDS_class-native = "hidapi-stm32mp-native"
DEPENDS_class-nativesdk = "nativesdk-hidapi-stm32mp"

RDEPENDS_${PN} += "libusb1 hidapi-stm32mp"

EXTRA_OECONF = " \
    --disable-doxygen-html \
    --disable-werror \
    --enable-stlink \
    --enable-cmsis-dap \
    --without-capstone \
    MAKEINFO=true \
    HIDAPI_CFLAGS=-I${STAGING_INCDIR}/hidapi \
    HIDAPI_LIBS=-L${STAGING_LIBDIR}\ -lhidapi-libusb \
"

# Can't be built with ccache
CCACHE_DISABLE = "1"

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
