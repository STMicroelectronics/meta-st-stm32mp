FILESEXTRAPATHS:append := "${THISDIR}/files:"

EXTRA_OEMAKE:append:task-compile = " JOBS=1"

PACKAGECONFIG:stm32mpcommon = "python tui libunwind coresight zstd"
PACKAGECONFIG:class-native = "python tui libunwind coresight zstd"
PACKAGECONFIG:class-nativesdk = "python tui libunwind coresight zstd"

FILES:${PN}:append:class-nativesdk = " ${base_prefix}/usr/etc/bash_completion.d "
FILES:${PN}:append:class-native = " ${sysconfdir}/bash_completion.d "

BBCLASSEXTEND += "native nativesdk"
SDK_CC_ARCH += "${DEBUG_PREFIX_MAP}"
BUILD_CC += "${DEBUG_PREFIX_MAP}"

RDEPENDS:${PN}:append:class-nativesdk = " nativesdk-llvm"
RDEPENDS:${PN}-python:append:class-nativesdk = " nativesdk-llvm"
