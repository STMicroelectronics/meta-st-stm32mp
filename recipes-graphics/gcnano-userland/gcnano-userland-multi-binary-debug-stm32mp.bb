require gcnano-userland-binary.inc

SUMMARY = "[DEBUG] Vivante libraries OpenGL ES, OpenVG and EGL (multi backend)"
LICENSE = "Proprietary"

GCNANO_BACKEND = "multi"
GCNANO_FLAVOUR = "debug"

PROVIDES:remove = "gcnano-userland"
