SUMMARY = "Capstone is a lightweight multi-platform, multi-architecture disassembly framework."
HOMEPAGE = "http://www.capstone-engine.org/index.html"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE.TXT;md5=1cfbff4f40612b0144e498a47c91499c"

DEPENDS = ""

SRC_URI = "git://github.com/capstone-engine/capstone.git;protocol=https;branch=v5"
SRCREV = "accf4df62f1fba6f92cae692985d27063552601c"

inherit cmake

EXTRA_OECMAKE += " -DBUILD_SHARED_LIBS=ON "

BBCLASSEXTEND += "native nativesdk"
