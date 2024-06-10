RDEPENDS:${PN}-tests =+ "bash"

PACKAGECONFIG:stm32mpcommon = "scripting tui libunwind coresight"
PACKAGECONFIG:class-native = "scripting tui libunwind coresight"
PACKAGECONFIG:class-nativesdk = "scripting tui libunwind coresight"

FILES:${PN}:append:class-nativesdk = " ${base_prefix}/usr/etc/bash_completion.d "
FILES:${PN}:append:class-native = " ${sysconfdir}/bash_completion.d "

BBCLASSEXTEND += "native nativesdk"
