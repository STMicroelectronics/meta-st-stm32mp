FILESEXTRAPATHS:prepend := "${THISDIR}/mesa:"

DEPENDS += "zstd"

PACKAGECONFIG:stm32mp1common = " \
    ${@bb.utils.filter('DISTRO_FEATURES', 'x11 wayland', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'opengl', 'opengl egl gles gbm', '', d)} \
    \
    gallium \
    etnaviv \
    zlib \
    xmlconfig \
    "

PACKAGECONFIG:stm32mp2common = " \
    ${@bb.utils.filter('DISTRO_FEATURES', 'x11 wayland vulkan', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'opengl', 'opengl egl gles gbm', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'opencl', 'opencl libclc gallium-llvm', '', d)} \
    gallium \
    etnaviv \
    zlib \
    xmlconfig \
    "

CFLAGS += "-Wno-pointer-to-int-cast -Wno-cpp"
