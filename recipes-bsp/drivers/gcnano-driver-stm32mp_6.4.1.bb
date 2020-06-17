SUMMARY = "GCNano kernel drivers"
LICENSE = "GPLv1 & MIT"
# Note get md5sum with: $ head -n 53 Makefile | md5sum
LIC_FILES_CHKSUM = "file://Makefile;endline=53;md5=c53e65c2dc344ddd2f74196aae9e8384"

SRC_URI = "git://github.com/STMicroelectronics/gcnano-binaries;protocol=https;branch=gcnano-6.4.1-binaries"
SRCREV = "eaff2c61bcc3936f8193a52ffe965bdcc6457a57"

GCNANO_VERSION = "6.4.1"

PV = "${GCNANO_VERSION}-tar.${SRCPV}"

S = "${WORKDIR}/gcnano-driver-${GCNANO_VERSION}"

include gcnano-driver-stm32mp.inc

GCNANO_DRIVER_TARBALL = "gcnano-driver-${GCNANO_VERSION}.tar.xz"

DEPENDS += "xz-native"
do_unpack[depends] += "xz-native:do_populate_sysroot"

python () {
    externalsrc = d.getVar('EXTERNALSRC')
    if not externalsrc:
        d.prependVarFlag('do_unpack', 'postfuncs', " do_patch_extract_tarball ")
}

# add to do_patch for untar the tarball
do_patch_extract_tarball() {
    bbnote "tar xfJ ${WORKDIR}/git/${GCNANO_DRIVER_TARBALL} -C ${WORKDIR}"
    tar xfJ ${WORKDIR}/git/${GCNANO_DRIVER_TARBALL} -C ${WORKDIR}
}
