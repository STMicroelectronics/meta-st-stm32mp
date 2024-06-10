FILESEXTRAPATHS:prepend := "${THISDIR}/mesa:"

PACKAGECONFIG:stm32mp1common = "${@bb.utils.filter('DISTRO_FEATURES', 'wayland ', d)} \
                 ${@bb.utils.contains('DISTRO_FEATURES', 'opengl', 'opengl egl gles gbm', '', d)} \
                 ${@bb.utils.contains('DISTRO_FEATURES', 'x11 opengl', 'x11', '', d)} \
                 ${@bb.utils.contains('DISTRO_FEATURES', 'x11 vulkan', 'dri3', '', d)} \
                \
                gallium \
        "

PACKAGECONFIG:stm32mp2common = " \
    gallium \
    ${@bb.utils.filter('DISTRO_FEATURES', 'wayland vulkan', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'opengl', 'opengl egl gles gbm', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'x11 opengl', 'x11 dri3', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'x11 vulkan', 'x11 dri3', '', d)} \
    "

PACKAGECONFIG = " \
	gallium \
	${@bb.utils.filter('DISTRO_FEATURES', 'x11 vulkan wayland', d)} \
	${@bb.utils.contains('DISTRO_FEATURES', 'opengl', 'opengl egl gles gbm virgl', '', d)} \
	${@bb.utils.contains('DISTRO_FEATURES', 'x11 opengl', 'dri3', '', d)} \
	${@bb.utils.contains('DISTRO_FEATURES', 'x11 vulkan', 'dri3', '', d)} \
"

# Enable Etnaviv support
GALLIUMDRIVERS:append:stm32mpcommon = ",etnaviv,kmsro"
