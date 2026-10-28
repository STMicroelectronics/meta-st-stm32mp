SUMMARY = "A Linux command-line tool for M33TD FWU"
LICENSE = "BSD-3-Clause"
# License file is inside the rpmsg_fwu subdirectory
LIC_FILES_CHKSUM = "file://LICENSE;md5=1787eff1a4fbd59457aa553b09394653"

SRC_URI = "git://github.com/STMicroelectronics/st-openstlinux-application.git;protocol=https;branch=main"
SRCREV = "62f0d0b0ea4bca81f55a3042284d47ceebe86919"

PV = "1.1+git-${@d.getVar('SRCREV')[0:8]}"

S = "${UNPACKDIR}/${BP}/rpmsg_fwu"

do_compile() {
	oe_runmake
}

do_install() {
	install -d ${D}${prefix}/local/demo/bin
	install -m 0755 ${B}/m33rpfwu ${D}${prefix}/local/demo/bin/m33rpfwu
}

FILES:${PN} = "${prefix}/local/demo/bin/m33rpfwu"
