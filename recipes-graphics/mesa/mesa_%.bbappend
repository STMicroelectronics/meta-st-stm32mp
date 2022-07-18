FILESEXTRAPATHS:prepend := "${THISDIR}/mesa:"

PACKAGECONFIG:stm32mpcommon = "${@bb.utils.filter('DISTRO_FEATURES', 'wayland ', d)} \
                 ${@bb.utils.contains('DISTRO_FEATURES', 'opengl', 'opengl egl gles gbm', '', d)} \
                 ${@bb.utils.contains('DISTRO_FEATURES', 'x11 opengl', 'x11', '', d)} \
                 ${@bb.utils.contains('DISTRO_FEATURES', 'x11 vulkan', 'dri3', '', d)} \
                \
                gallium \
        "

# Enable Etnaviv support
GALLIUMDRIVERS:append:stm32mpcommon = ",etnaviv,kmsro"
