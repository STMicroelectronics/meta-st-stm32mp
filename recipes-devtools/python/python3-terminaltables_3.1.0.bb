SUMMARY = "Python3 compatibility library"
HOMEPAGE = "https://pypi.org/project/terminaltables/"
SECTION = "devel/python"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://PKG-INFO;beginline=8;endline=8;md5=8227180126797a0148f94f483f3e1489"

SRC_URI[md5sum] = "863797674d8f75d22e16e6c1fdcbeb41"
SRC_URI[sha256sum] = "f3eb0eb92e3833972ac36796293ca0906e998dc3be91fbe1f8615b331b853b81"

inherit pypi

BBCLASSEXTEND += "native nativesdk"

inherit setuptools3

