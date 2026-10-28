PACKAGE_ARCH:stm32mpcommon = "${MACHINE_ARCH}"

do_install:append:stm32mpcommon() {
    rm -rf ${D}/usr/include/KHR
}
