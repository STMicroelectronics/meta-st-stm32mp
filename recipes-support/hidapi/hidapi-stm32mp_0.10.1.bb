SUMMARY = "Multi-platform library to interface with USB and Bluetooth HID-Class devices"
AUTHOR = "Alan Ott"
HOMEPAGE = "http://www.signal11.us/oss/hidapi/"
SECTION = "libs"

LICENSE = "BSD-3-Clause | GPLv3"
LIC_FILES_CHKSUM = "file://LICENSE.txt;md5=7c3949a631240cb6c31c50f3eb696077"

SRC_URI = "git://github.com/libusb/hidapi.git;protocol=https \
           file://0001-configure.ac-remove-duplicate-AC_CONFIG_MACRO_DIR-22.patch \
"
SRCREV = "f6d0073fcddbdda24549199445e844971d3c9cef"

PV = "0.10.1-git.${SRCPV}"

S = "${WORKDIR}/git"

inherit autotools pkgconfig

BBCLASSEXTEND += "native nativesdk"

DEPENDS += "libusb"

DEPENDS_class-native += "libusb-native"
DEPENDS_class-nativesdk += "nativesdk-libusb"

# Disable udev backend build in native/nativesdk
EXTRA_OECONF += "libudev_CFLAGS=' ' libudev_LIBS=' '"
EXTRA_OEMAKE += "SUBDIRS=libusb"

EXTRA_OECONF_class-native += "libudev_CFLAGS=' ' libudev_LIBS=' '"
EXTRA_OEMAKE_class-native += "SUBDIRS=libusb"

EXTRA_OECONF_class-nativesdk += "libudev_CFLAGS=' ' libudev_LIBS=' '"
EXTRA_OEMAKE_class-nativesdk += "SUBDIRS=libusb"
