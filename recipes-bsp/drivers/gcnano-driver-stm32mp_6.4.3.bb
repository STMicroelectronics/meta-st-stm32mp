SUMMARY = "GCNano kernel drivers"
LICENSE = "GPLv1 & MIT"
# Note get md5sum with: $ head -n 53 Makefile | md5sum
LIC_FILES_CHKSUM = "file://Makefile;endline=53;md5=3ad4e99418a2e0f339ecdc29ac648351"

SRC_URI = "git://github.com/STMicroelectronics/gcnano-binaries;protocol=https;branch=gcnano-6.4.3-binaries"
SRCREV = "1534c3eaabb5ae545a8f97e95f853531365a13fc"

GCNANO_VERSION = "6.4.3"

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
