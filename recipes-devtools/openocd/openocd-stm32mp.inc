DEPENDS = "hidapi-stm32mp capstone"

CVE_PRODUCT = "openocd:open_on-chip_debugger"

inherit pkgconfig autotools-brokensep gettext

SRC_URI:append = " \
    git://github.com/msteveb/jimtcl.git;protocol=https;destsuffix=${BP}/jimtcl;nobranch=1;name=jimtcl                         \
    git://gitlab.zapb.de/libjaylink/libjaylink.git;protocol=https;destsuffix=${BP}/src/jtag/drivers/libjaylink;nobranch=1;name=libjaylink \
    "

SRCREV_jimtcl = "3920cedd1cdd4702720773f0637b780f79be6158"
SRCREV_libjaylink = "9aa7a5957c07bb6e862fc1a6d3153d109c7407e4"

BBCLASSEXTEND += "native nativesdk"

DEPENDS:class-native = "hidapi-stm32mp-native capstone-native"
DEPENDS:class-nativesdk = "nativesdk-hidapi-stm32mp nativesdk-capstone"

RDEPENDS:${PN} += "libusb1 hidapi-stm32mp capstone"

EXTRA_OECONF = " \
    --disable-doxygen-html \
    --disable-werror \
    --enable-stlink \
    --enable-cmsis-dap \
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
