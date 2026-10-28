FILESEXTRAPATHS:prepend := "${THISDIR}/mesa:"

DEPENDS += "zstd"

PACKAGECONFIG:stm32mp1common = " \
    expat \
    gallium \
    ${@bb.utils.filter('DISTRO_FEATURES', 'x11 wayland', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'opengl', 'opengl egl gles gbm', '', d)} \
    ${@bb.utils.contains_any('DISTRO_FEATURES', 'opengl vulkan', 'virtio', '', d)} \
    \
    xmlconfig \
    zlib \
    \
    etnaviv \
    "

PACKAGECONFIG:stm32mp2common = " \
    expat \
    gallium \
    ${@bb.utils.filter('DISTRO_FEATURES', 'x11 wayland vulkan', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'opengl', 'opengl egl gles gbm', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'opencl', 'opencl libclc gallium-llvm', '', d)} \
    ${@bb.utils.contains_any('DISTRO_FEATURES', 'opengl vulkan', 'virtio', '', d)} \
    xmlconfig \
    zlib \
    \
    etnaviv \
    "

CFLAGS += "-Wno-pointer-to-int-cast -Wno-cpp"
