SUMMARY = "Baremetal GCC for ARM"
LICENSE = "GPL-3.0-with-GCC-exception & GPL-3.0-only"

require gcc-arm-none-eabi_${PV}.inc

inherit nativesdk

FILES:${PN}-doc = "${datadir}/gcc-arm-none-eabi/share/doc"

FILES:${PN}-staticdev = "${datadir}/gcc-arm-none-eabi/*/*/*.a \
                         ${datadir}/gcc-arm-none-eabi/*/*/*/*.a \
                         ${datadir}/gcc-arm-none-eabi/*/*/*/*/*.a \
                         ${datadir}/gcc-arm-none-eabi/*/*/*/*/*/*.a \
                         ${datadir}/gcc-arm-none-eabi/*/*/*/*/*/*/*.a \
                         ${datadir}/gcc-arm-none-eabi/*/*/*/*/*/*/*/*.a \
                         ${datadir}/gcc-arm-none-eabi/*/*/*/*/*/*/*/*/*.a \
                        "

FILES:${PN} += "${datadir}/gcc-arm-none-eabi"

# Some library files in the tarball are not at the expected place,
# and it's working. But QA will complain, so skip the complaint on libdir
INSANE_SKIP:${PN} += "libdir"
