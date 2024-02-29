FILESEXTRAPATHS:prepend:stm32mpcommon := "${THISDIR}/gcc:"
SRC_URI += " \
    file://0031-make-gcc-plugins-work-for-the-sdk.patch \
"
