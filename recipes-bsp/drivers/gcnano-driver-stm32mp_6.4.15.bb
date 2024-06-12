SUMMARY = "GCNano kernel drivers"
LICENSE = "GPL-1.0-only & MIT"
# Note get md5sum with: $ head -n 53 Makefile | md5sum
LIC_FILES_CHKSUM = "file://Makefile;endline=53;md5=85a35fecd70aaa9c4047554f71e1407d"

SRC_URI = "git://github.com/STMicroelectronics/gcnano-binaries;protocol=https;branch=gcnano-${GCNANO_VERSION}-binaries"
SRCREV = "bbaae49a0e4859ed53f898a250269c8a237261bc"

GCNANO_VERSION = "6.4.15"
GCNANO_SUBVERSION:stm32mp1common = "stm32mp1"
GCNANO_SUBVERSION:stm32mp2common = "stm32mp2"
GCNANO_RELEASE = "r1"

PV = "${GCNANO_VERSION}-${GCNANO_SUBVERSION}-${GCNANO_RELEASE}"

S = "${WORKDIR}/git/${BPN}"

include gcnano-driver-stm32mp.inc

# ---------------------------------
# Configure archiver use
# ---------------------------------
include ${@oe.utils.ifelse(d.getVar('ST_ARCHIVER_ENABLE') == '1', 'gcnano-driver-stm32mp-archiver.inc','')}
