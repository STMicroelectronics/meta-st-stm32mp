SUMMARY = "Baremetal GCC for ARM aarch64"
LICENSE = "GPL-3.0-with-GCC-exception & GPL-3.0-only"

require gcc-aarch64-none-elf_${PV}.inc

inherit nativesdk

FILES:${PN}-doc = "${datadir}/gcc-arm-none-eabi/share/doc"

FILES:${PN}-staticdev = "${datadir}/gcc-aarch64-none-elf/*/*/*.a \
                         ${datadir}/gcc-aarch64-none-elf/*/*/*/*.a \
                         ${datadir}/gcc-aarch64-none-elf/*/*/*/*/*.a \
                         ${datadir}/gcc-aarch64-none-elf/*/*/*/*/*/*.a \
                         ${datadir}/gcc-aarch64-none-elf/*/*/*/*/*/*/*.a \
                         ${datadir}/gcc-aarch64-none-elf/*/*/*/*/*/*/*/*.a \
                         ${datadir}/gcc-aarch64-none-elf/*/*/*/*/*/*/*/*/*.a \
                        "

FILES:${PN} += "${datadir}/gcc-aarch64-none-elf"

# Some library files in the tarball are not at the expected place,
# and it's working. But QA will complain, so skip the complaint on libdir
INSANE_SKIP:${PN} += "libdir"
INSANE_SKIP:${PN}-dbg:class-nativesdk += "libdir"
INSANE_SKIP:${PN}:class-nativesdk += "libdir arch"
INSANE_SKIP:${PN}:class-nativesdk += "already-stripped file-rdeps buildpaths"
