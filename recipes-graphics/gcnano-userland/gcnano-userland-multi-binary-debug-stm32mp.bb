# Recipe for installing gcnano-userland binaries (gbm backend)
SUMMARY = "[DEBUG] Vivante libraries OpenGL ES, OpenVG and EGL (multi backend)"
LICENSE = "Proprietary"

BACKEND = "multi"

DEPENDS += " libdrm wayland "

GCNANO_TYPE = "debug"

do_install:append() {
    clean_release_file
}

include gcnano-userland-binary.inc

PROVIDES:remove = "gcnano-userland"
