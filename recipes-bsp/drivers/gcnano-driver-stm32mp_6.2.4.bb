SUMMARY = "GCNano kernel drivers"
LICENSE = "GPLv1 & MIT"
# Note get md5sum with: $ head -n 53 Makefile | md5sum
LIC_FILES_CHKSUM = "file://Makefile;endline=53;md5=d77ff5896dbbf8a8bc3f7c5e8f905fcc"

SRC_URI = "git://github.com/STMicroelectronics/gcnano-binaries;protocol=https;branch=gcnano-6.2.4_p3-binaries"
SRCREV = "271f87d816a957bf196f6328c34110cab1224d4d"

PV = "6.2.4.p3"
PR = "tar${SRCPV}"

S = "${WORKDIR}/gcnano-driver-${PV}"

include gcnano-driver-stm32mp.inc

GCNANO_DRIVER_TARBALL = "gcnano-driver-${PV}.tar.xz"

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
