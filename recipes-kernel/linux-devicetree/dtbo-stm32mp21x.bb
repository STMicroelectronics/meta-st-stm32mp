SUMMARY = "Device tree overlay for stm32mp21x board"
DESCRIPTION = "Device tree overlay for stm32mp21x board"
HOMEPAGE = "www.st.com"

LICENSE = "GPL-2.0-only | BSD-3-Clause"
LIC_FILES_CHKSUM = " \
    file://${COREBASE}/meta/files/common-licenses/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6 \
    file://${COREBASE}/meta/files/common-licenses/BSD-3-Clause;md5=550794465ba0ec5312d6919e203a55f9 \
"
COMPATIBLE_MACHINE = "(stm32mp2common)"
SRC_URI = " \
    file://stm32mp215f-dk-m2-bcm43xx-2ae.dts \
"

inherit devicetree
PROVIDES:remove = "virtual/dtb"

