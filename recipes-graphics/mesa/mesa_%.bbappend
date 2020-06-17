FILESEXTRAPATHS_prepend := "${THISDIR}/mesa:"

PACKAGECONFIG_stm32mpcommon = "${@bb.utils.filter('DISTRO_FEATURES', 'wayland ', d)} \
                 ${@bb.utils.contains('DISTRO_FEATURES', 'opengl', 'opengl egl gles gbm dri', '', d)} \
                 ${@bb.utils.contains('DISTRO_FEATURES', 'x11 opengl', 'x11', '', d)} \
                 ${@bb.utils.contains('DISTRO_FEATURES', 'x11 vulkan', 'dri3', '', d)} \
                \
                gallium \
        "

# Enable Etnaviv support
GALLIUMDRIVERS_append_stm32mpcommon = ",etnaviv,kmsro"
