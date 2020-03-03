# Recipe for installing gcnano-userland binaries (gbm backend)
SUMMARY = "[DEBUG] Vivante libraries OpenGL ES, OpenVG and EGL (multi backend)"
LICENSE = "Proprietary"

BACKEND = "multi"

DEPENDS += " libdrm wayland "

GCNANO_TYPE = "debug"

do_install_append() {
    clean_release_file
}

include gcnano-userland-binary.inc

PROVIDES_remove = "gcnano-userland"
