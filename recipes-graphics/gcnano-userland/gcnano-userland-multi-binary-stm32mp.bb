# Recipe for installing gcnano-userland binaries (gbm backend)
SUMMARY = "Vivante libraries OpenGL ES, OpenVG and EGL (multi backend)"
LICENSE = "Proprietary"

BACKEND = "multi"

DEPENDS += " libdrm wayland "

GCNANO_TYPE = "release"

do_install_append() {
    clean_debug_file
}

include gcnano-userland-binary.inc
