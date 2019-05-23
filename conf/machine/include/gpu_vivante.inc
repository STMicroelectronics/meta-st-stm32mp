# =========================================================================
# GPU
# =========================================================================
GPU_USERLAND_LIBRARIES_WAYLAND ?= "gcnano-userland-multi-binary-stm32mp"
GPU_USERLAND_LIBRARIES_EGLFS   ?= "gcnano-userland-multi-binary-stm32mp"
GPU_USERLAND_LIBRARIES_X11     ?= "mesa"

# Helper function for overloading the default EGL/GLES implementation.
# The Vivante libraries are compatible with the Mesa headers
# but we can have several backend available following the distro configuration:
#  - wayland
#  - x11
#  - wayland + x11
#  - framebuffer
#  - drm

def get_gpu_vivante_handler(d):
    """  Overloading the default EGL/GLES/mesa implementation."""
    machine_features = d.getVar('MACHINE_FEATURES').split()
    distro_features = d.getVar('DISTRO_FEATURES').split()

    gpu_lib_wayland = d.getVar('GPU_USERLAND_LIBRARIES_WAYLAND').split()
    gpu_lib_eglfs = d.getVar('GPU_USERLAND_LIBRARIES_EGLFS').split()
    gpu_lib_x11 = d.getVar('GPU_USERLAND_LIBRARIES_X11').split()

    if 'gpu' in machine_features:
        if 'wayland' in distro_features:
            provider = gpu_lib_wayland[0]
        else:
            if 'x11' in distro_features:
                provider = gpu_lib_x11[0]
            else:
                '''no wayland, no X11 -> choose DRM/FB for eglfs'''
                provider = gpu_lib_eglfs[0]
    else:
        provider = "mesa"

    return provider;

GPU_USERLAND_LIBRARIES_INSTALL = "${@get_gpu_vivante_handler(d)}"

PREFERRED_PROVIDER_virtual/egl = "${@get_gpu_vivante_handler(d)}"
PREFERRED_PROVIDER_virtual/libgles1 = "${@get_gpu_vivante_handler(d)}"
PREFERRED_PROVIDER_virtual/libgles2 = "${@get_gpu_vivante_handler(d)}"
PREFERRED_PROVIDER_virtual/libgbm = "${@get_gpu_vivante_handler(d)}"
PREFERRED_PROVIDER_virtual/mesa = "${@bb.utils.contains('PREFERRED_PROVIDER_virtual/egl','mesa','mesa','mesa-gl',d)}"
PREFERRED_PROVIDER_virtual/libgl = "${@bb.utils.contains('PREFERRED_PROVIDER_virtual/egl','mesa','mesa','mesa-gl',d)}"
