FILESEXTRAPATHS:prepend := "${THISDIR}/mesa:"

PACKAGECONFIG:stm32mp1common = " \
    ${@bb.utils.filter('DISTRO_FEATURES', 'wayland ', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'opengl', 'opengl egl gles gbm', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'x11 opengl', 'dri3', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'x11 vulkan', 'dri3', '', d)} \
    \
    gallium \
    etnaviv \
    kmsro \
    "

PACKAGECONFIG:stm32mp2common = " \
    gallium \
    ${@bb.utils.filter('DISTRO_FEATURES', 'x11 wayland vulkan', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'opengl', 'opengl egl gles gbm', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'x11 opengl', 'dri3', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'x11 vulkan', 'dri3', '', d)} \
    etnaviv \
    kmsro \
    "
