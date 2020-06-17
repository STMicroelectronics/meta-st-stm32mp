# Usage: add INHERIT += "image-gcnano-link" to your conf file

GCNANO_USERLAND_VENDOR_DIR ?= "/vendor"
GCNANO_USERLAND_OUTPUT_LIBDIR = "${@'${GCNANO_USERLAND_VENDOR_DIR}/lib' if d.getVar('GCNANO_USERLAND_USE_VENDOR_DIR') == '1' else '${libdir}'}"

gcnano_create_link() {
    if [ -L ${IMAGE_ROOTFS}${libdir}/libEGL.so ];
    then
        # link requested:
        # libEGL.so.1 -> /vendor/lib/libEGL.so
        if [ ! -L ${IMAGE_ROOTFS}/usr/lib/libEGL.so.1 ];
        then
            LINK=$(readlink ${IMAGE_ROOTFS}${libdir}/libEGL.so)
            ln -s $LINK ${IMAGE_ROOTFS}${libdir}/libEGL.so.1
        fi
        # link requested:
        # libgbm.so.1 -> /vendor/lib/libgbm.so
        if [ ! -L ${IMAGE_ROOTFS}${libdir}/libgbm.so.1 ];
        then
            LINK=$(readlink ${IMAGE_ROOTFS}${libdir}/libgbm.so)
            ln -s $LINK ${IMAGE_ROOTFS}${libdir}/libgbm.so.1
        fi
        # link requested:
        # libGLESv2.so.2 -> /vendor/lib/libGLESv2.so
        if [ ! -L ${IMAGE_ROOTFS}${libdir}/libGLESv2.so.2 ];
        then
            LINK=$(readlink ${IMAGE_ROOTFS}${libdir}/libGLESv2.so)
            ln -s $LINK ${IMAGE_ROOTFS}${libdir}/libGLESv2.so.2
        fi
    fi
}

IMAGE_PREPROCESS_COMMAND += "gcnano_create_link;"
