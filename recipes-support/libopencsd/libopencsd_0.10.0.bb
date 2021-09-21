SUMMARY = "CoreSight Trace Decode library"
DESCRIPTION = "An open source CoreSight Trace Decode library"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE;md5=ad8cb685eb324d2fa2530b985a43f3e5"

inherit autotools gettext pkgconfig

SRC_URI = "https://github.com/Linaro/OpenCSD/archive/v${PV}.tar.gz"
SRC_URI[md5sum] = "137abb1e606ec8c6707a80e4a4a0d3b7"
SRC_URI[sha256sum] = "33d07aa2205c956d508ecfa9eec6f25bd2c6206571466c60f247640bee33eaea"
SRC_URI += "file://0001-add-autotools-support.patch"

S = "${WORKDIR}/OpenCSD-${PV}"

do_install:append() {
	install -d ${D}${includedir}/opencsd
	cp ${S}/decoder/include/opencsd/ocsd_if_types.h ${D}${includedir}/opencsd
	cp ${S}/decoder/include/opencsd/ocsd_if_version.h ${D}${includedir}/opencsd
	cp ${S}/decoder/include/opencsd/trc_gen_elem_types.h ${D}${includedir}/opencsd
	cp ${S}/decoder/include/opencsd/trc_pkt_types.h ${D}${includedir}/opencsd

	install -d ${D}${includedir}/opencsd/ptm
	cp ${S}/decoder/include/opencsd/ptm/trc_pkt_types_ptm.h ${D}${includedir}/opencsd/ptm

	install -d ${D}${includedir}/opencsd/stm
	cp ${S}/decoder/include/opencsd/stm/trc_pkt_types_stm.h ${D}${includedir}/opencsd/stm

	install -d ${D}${includedir}/opencsd/etmv3
	cp ${S}/decoder/include/opencsd/etmv3/trc_pkt_types_etmv3.h ${D}${includedir}/opencsd/etmv3

	install -d ${D}${includedir}/opencsd/etmv4
	cp ${S}/decoder/include/opencsd/etmv4/trc_pkt_types_etmv4.h ${D}${includedir}/opencsd/etmv4

	install -d ${D}${includedir}/opencsd/c_api
	cp ${S}/decoder/include/opencsd/c_api/ocsd_c_api_types.h ${D}${includedir}/opencsd/c_api
	cp ${S}/decoder/include/opencsd/c_api/opencsd_c_api.h ${D}${includedir}/opencsd/c_api
	cp ${S}/decoder/include/opencsd/c_api/ocsd_c_api_custom.h ${D}${includedir}/opencsd/c_api
}
