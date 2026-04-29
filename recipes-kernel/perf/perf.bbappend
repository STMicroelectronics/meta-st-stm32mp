FILESEXTRAPATHS:append := "${THISDIR}/files:"

PACKAGECONFIG:stm32mpcommon = "python tui libunwind coresight"
PACKAGECONFIG:class-native = "python tui libunwind coresight"
PACKAGECONFIG:class-nativesdk = "python tui libunwind coresight"

FILES:${PN}:append:class-nativesdk = " ${base_prefix}/usr/etc/bash_completion.d "
FILES:${PN}:append:class-native = " ${sysconfdir}/bash_completion.d "

BBCLASSEXTEND += "native nativesdk"
SDK_CC_ARCH += "${DEBUG_PREFIX_MAP}"
BUILD_CC += "${DEBUG_PREFIX_MAP}"

RDEPENDS:${PN}:append:class-nativesdk = " nativesdk-llvm"
RDEPENDS:${PN}-python:append:class-nativesdk = " nativesdk-llvm"
