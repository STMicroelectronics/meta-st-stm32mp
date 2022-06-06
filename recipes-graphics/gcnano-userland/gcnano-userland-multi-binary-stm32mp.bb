require gcnano-userland-binary.inc

SUMMARY = "Vivante libraries OpenGL ES, OpenVG and EGL (multi backend)"
LICENSE = "Proprietary"

GCNANO_PACKAGECONFIG = "egl gbm glesv1 glesv2 vg"

GCNANO_BACKEND = "multi"
GCNANO_FLAVOUR = "release"
