SUMMARY = "A New System Troubleshooting Tool Built for the Way You Work"
DESCRIPTION = "Sysdig is open source, system-level exploration: capture \
system state and activity from a running Linux instance, then save, \
filter and analyze."
HOMEPAGE = "http://www.sysdig.org/"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=f8fee3d59797546cffab04f3b88b2d44"

PROVIDES_append_stm32mpcommon += " sysdig "

inherit cmake pkgconfig

OECMAKE_GENERATOR = "Unix Makefiles"

DEPENDS = "luajit zlib ncurses"
DEPENDS += "jsoncpp openssl curl jq"
DEPENDS += "tbb elfutils"
DEPENDS += "libb64"

RDEPENDS_${PN} = "bash"

SRC_URI = "git://github.com/draios/sysdig.git;protocol=https"
SRCREV = "aa82b2fb329ea97a8ade31590954ddaa675e1728"

PV = "0.24.2+git${SRCPV}"

S = "${WORKDIR}/git"

DIR_ETC="/etc"
EXTRA_OECMAKE = ' -DUSE_BUNDLED_LUAJIT="OFF" \
                  -DUSE_BUNDLED_ZLIB="OFF" \
                  -DBUILD_DRIVER="OFF" \
                  -DUSE_BUNDLED_NCURSES="OFF" \
                  -DDIR_ETC="${DIR_ETC}" \
                '
EXTRA_OECMAKE += ' \
    -DUSE_BUNDLED_LUAJIT="OFF" \
    -DUSE_BUNDLED_ZLIB="OFF" \
    -DUSE_BUNDLED_NCURSES="OFF" \
    -DUSE_BUNDLED_JSONCPP="OFF" \
    -DUSE_BUNDLED_OPENSSL="OFF" \
    -DUSE_BUNDLED_CURL="OFF" \
    -DUSE_BUNDLED_B64="OFF" \
    -DUSE_BUNDLED_JQ="OFF" \
    -DUSE_BUNDLED_TBB="OFF" \
    '

FILES_${PN} += " \
    ${DIR_ETC}/* \
    ${datadir}/zsh/* \
"
FILES_${PN}-dev = " ${prefix}/src/* "

# luajit not supported on Aarch64
COMPATIBLE_HOST = "^(?!aarch64).*"

