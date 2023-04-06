DEPENDS = "hidapi-stm32mp"

RDEPENDS:${PN} += "libusb1 hidapi-stm32mp"

inherit pkgconfig autotools-brokensep gettext

SRC_URI = "git://github.com/openocd-org/openocd.git;protocol=https;branch=master;name=openocd "
SRC_URI:append = " \
    git://git.savannah.nongnu.org/git/git2cl.git;protocol=https;;destsuffix=git/tools/git2cl;nobranch=1;name=git2cl         \
    git://github.com/msteveb/jimtcl.git;protocol=https;destsuffix=git/jimtcl;nobranch=1;name=jimtcl                         \
    git://gitlab.zapb.de/libjaylink/libjaylink.git;protocol=https;destsuffix=git/src/jtag/drivers/libjaylink;nobranch=1;name=libjaylink \
    "

S = "${WORKDIR}/git"

BBCLASSEXTEND += "native nativesdk"

DEPENDS:class-native = "hidapi-stm32mp-native"
DEPENDS:class-nativesdk = "nativesdk-hidapi-stm32mp"

RDEPENDS:${PN} += "libusb1 hidapi-stm32mp"

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

FILES:${PN} = " \
  ${datadir}/openocd/* \
  ${bindir}/openocd \
  "
