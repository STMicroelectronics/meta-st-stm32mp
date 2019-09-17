# Recipe for installing gcnano-userland binaries (gbm backend)
SUMMARY = "Vivante libraries OpenGL ES, OpenVG and EGL (multi backend)"
LICENSE = "Proprietary"

BACKEND = "multi"

DEPENDS += " libdrm wayland "

GCNANO_TYPE = "release"

GCNANO_USERLAND_FB_TARBALL_DATE = "20190328"

do_install_append() {
    clean_debug_file
}

include gcnano-userland-binary.inc
