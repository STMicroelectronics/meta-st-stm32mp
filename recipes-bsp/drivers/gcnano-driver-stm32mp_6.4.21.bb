SUMMARY = "GCNano kernel drivers"
LICENSE = "GPL-2.0-only & MIT"
# Note get md5sum with: $ head -n 19 Makefile | md5sum
LIC_FILES_CHKSUM = "file://Makefile;endline=19;md5=9b60522ab24b25fa2833058e76c41216"

SRC_URI = "git://github.com/STMicroelectronics/gcnano-binaries.git;protocol=https;branch=gcnano-${GCNANO_VERSION}-binaries"
SRCREV = "c981b5605ac964e0700ca84eb379407f92f8ce6d"

GCNANO_VERSION = "6.4.21"
GCNANO_RELEASE = "r2"

PV = "${GCNANO_VERSION}-${GCNANO_RELEASE}"

S = "${UNPACKDIR}/${BP}/${BPN}"

include gcnano-driver-stm32mp.inc

# ---------------------------------
# Configure archiver use
# ---------------------------------
include ${@oe.utils.ifelse(d.getVar('ST_ARCHIVER_ENABLE') == '1', 'gcnano-driver-stm32mp-archiver.inc','')}
