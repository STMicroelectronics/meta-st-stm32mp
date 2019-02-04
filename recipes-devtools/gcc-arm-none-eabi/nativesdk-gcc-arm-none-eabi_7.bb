SUMMARY = "Baremetal GCC for ARM"
LICENSE = "GPL-3.0-with-GCC-exception & GPLv3"

require gcc-arm-none-eabi_${PV}.inc

inherit nativesdk

FILES_${PN} += "${datadir}/gcc-arm-none-eabi"
