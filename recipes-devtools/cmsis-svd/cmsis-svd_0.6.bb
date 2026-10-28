SUMMARY = "CMSIS SVD parser"
HOMEPAGE = "https://pypi.org/project/cmsis_svd"

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://MANIFEST.in;md5=c6b0d37a81a72d681487ae797d48b3f4"

inherit pypi python_setuptools_build_meta

PYPI_PACKAGE = "cmsis_svd"

SRC_URI[md5sum] = "070c9a0c323e0fba1a6d398eee242e00"
SRC_URI[sha256sum] = "70cc58306f801f99a5ad6b49a8b3b4e9cca7d06b0aed77081facd69a84064686"

BBCLASSEXTEND += "native nativesdk"

# Make sure also to add python dependencies
RDEPENDS:${PN}:append = "python3-lxml"
