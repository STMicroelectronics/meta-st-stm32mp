FILESEXTRAPATHS:prepend := "${THISDIR}/u-boot-stm32mp:"

require u-boot-stm32mp-common_2022.10.inc

PACKAGES += "${PN}-mkfwumdata"

inherit python3native
DEPENDS += "swig-native gnutls util-linux openssl"
export STAGING_INCDIR="${STAGING_INCDIR_NATIVE}"

EXTRA_OEMAKE:class-target = 'CROSS_COMPILE="${TARGET_PREFIX}" CC="${CC} ${CFLAGS} ${LDFLAGS}" HOSTCC="${BUILD_CC} ${BUILD_CFLAGS} ${BUILD_LDFLAGS}" STRIP=true V=1'
EXTRA_OEMAKE:class-native = 'CC="${BUILD_CC} ${BUILD_CFLAGS} ${BUILD_LDFLAGS}" HOSTCC="${BUILD_CC} ${BUILD_CFLAGS} ${BUILD_LDFLAGS}" STRIP=true V=1'
EXTRA_OEMAKE:class-nativesdk = 'CROSS_COMPILE="${HOST_PREFIX}" CC="${CC} ${CFLAGS} ${LDFLAGS} " HOSTCC="${BUILD_CC} ${BUILD_CFLAGS} ${BUILD_LDFLAGS}" STRIP=true V=1'

SED_CONFIG_EFI = '-e "s/CONFIG_EFI_LOADER=.*/# CONFIG_EFI_LOADER is not set/"'
SED_CONFIG_EFI:x86 = ''
SED_CONFIG_EFI:x86-64 = ''
SED_CONFIG_EFI:arm = ''
SED_CONFIG_EFI:armeb = ''
SED_CONFIG_EFI:aarch64 = ''
SED_CONFIG_EFI:loongarch64 = ''

B = "${WORKDIR}/build"
do_compile() {
    # Yes, this is crazy. If you build on a system with git < 2.14 from scratch, the tree will
    # be marked as "dirty" and the version will include "-dirty", leading to a reproducibility problem.
    # The issue is the inode count for Licnses/README changing due to do_populate_lic hardlinking a
    # copy of the file. We avoid this by ensuring the index is updated with a "git diff" before the
    # u-boot machinery tries to determine the version.
    #
    # build$ ../git/scripts/setlocalversion ../git
    # ""
    # build$ ln ../git/
    # build$ ln ../git/README ../foo
    # build$ ../git/scripts/setlocalversion ../git
    # ""-dirty
    # (i.e. creating a hardlink dirties the index)
    cd ${S}; git diff; cd ${B}

    oe_runmake -C ${S} tools-only_defconfig O=${B}

    # Disable CONFIG_CMD_LICENSE, license.h is not used by tools and
    # generating it requires bin2header tool, which for target build
    # is built with target tools and thus cannot be executed on host.
    sed -i -e "s/CONFIG_CMD_LICENSE=.*/# CONFIG_CMD_LICENSE is not set/" ${SED_CONFIG_EFI} ${B}/.config

    oe_runmake -C ${S} cross_tools NO_SDL=1 O=${B}
}

do_install() {
	install -d ${D}${bindir}

	# mkimage
	install -m 0755 tools/mkfwumdata ${D}${bindir}/
}

ALLOW_EMPTY:${PN} = "1"
FILES:${PN} = ""
FILES:${PN}-mkfwumdata = "${bindir}/mkfwumdata"

RDEPENDS:${PN} += "${PN}-mkfwumdata"

BBCLASSEXTEND = "native nativesdk"
