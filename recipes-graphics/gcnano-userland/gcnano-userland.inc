PACKAGE_ARCH = "${MACHINE_ARCH}"

GCNANO_PACKAGECONFIG ??= ""

PROVIDES = "\
    gcnano-userland \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'egl', 'virtual/egl', '', d)}          \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'gbm', 'virtual/libgbm', '', d)}       \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'glesv1', 'virtual/libgles1', '', d)}  \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'glesv2', 'virtual/libgles2', '', d)}  \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'glesv2', 'virtual/libgles3', '', d)}  \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'vg', 'virtual/libopenvg', '', d)}     \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'vx', 'virtual/libopenvx', '', d)}     \
    "

PACKAGES += "\
    gcnano-ldconf                                                                                           \
    libgal-gcnano libgal-gcnano-dev                                                                         \
    \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'cl', 'libopencl-gcnano libopencl-gcnano-dev', '', d)}     \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'egl', 'libegl-gcnano libegl-gcnano-dev', '', d)}          \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'gbm', 'libgbm-gcnano libgbm-gcnano-dev', '', d)}          \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'glesv1', 'libgles1-gcnano libgles1-gcnano-dev', '', d)}   \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'glesv2', 'libgles2-gcnano libgles2-gcnano-dev', '', d)}   \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'glesv2', 'libgles3-gcnano-dev', '', d)}                   \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'vg', 'libopenvg-gcnano libopenvg-gcnano-dev', '', d)}     \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'vulkan', 'libvulkan-driver-gcnano libvulkan-driver-gcnano-dev', '', d)} \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'vx', 'libopenvx-gcnano libopenvx-gcnano-dev', '', d)}     \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'vx', 'libovxkernels-gcnano libovxkernels-gcnano-dev', '', d)} \
    \
    ${@bb.utils.contains_any('GCNANO_PACKAGECONFIG', 'glesv1 glesv2 vg vulkan', 'libglslc-gcnano libglslc-gcnano-dev', '', d)}  \
    ${@bb.utils.contains_any('GCNANO_PACKAGECONFIG', 'cl vx', 'libclc-gcnano libclc-gcnano-dev', '', d)}                        \
    ${@bb.utils.contains_any('GCNANO_PACKAGECONFIG', 'cl vulkan', 'libspirv-gcnano libspirv-gcnano-dev', '', d)}    \
    "

GCNANO_BACKEND ??= "multi"
GCNANO_VERSION ??= "6"
GCNANO_DATE ??= ""

GCNANO_MACHINE:stm32mp1common = "stm32mp1"
GCNANO_MACHINE:stm32mp2common = "stm32mp2"

# Init the gcnano build type
GCNANO_FLAVOUR ??= "release"

# Init the gcnano tarball file name
GCNANO_TAR_FILENAME ?= "gcnano-userland-${GCNANO_BACKEND}-${GCNANO_MACHINE}-${GCNANO_VERSION}-${GCNANO_DATE}"

# Configure gcnano output library dir
GCNANO_USERLAND_OUTPUT_LIBDIR ??= "${libdir}"

GCNANO_USERLAND_LDCONF ?= "gcnano.conf"
GCNANO_OPENCL_ICD ?= "VeriSilicon.icd"
GCNANO_VULKAN_ICD ?= "VeriSilicon_icd.json"

# Configure the pkgconfig settings
PKGCONFIG_DV ??= "23.0.3"
PKGCONFIG_PREFIX ??= "${exec_prefix}"

do_compile:append() {
    # Generate specific conf file if required
    if [ "${GCNANO_USERLAND_OUTPUT_LIBDIR}" = "${libdir}" ]; then
        bbnote "gcnano-userland output libdir is default one (${libdir})"
    else
        bbnote "gcnano-userland output libdir set to ${GCNANO_USERLAND_OUTPUT_LIBDIR}: generate specific conf file for ldconfig"
        echo ${GCNANO_USERLAND_OUTPUT_LIBDIR} > ${B}/${GCNANO_USERLAND_LDCONF}
    fi
}

# Install utility for gcano-userland:
#   gcnano_install_lib  <LIBDIR>  <LIBDIR_INSTALL> \
#                       <INCDIR>  <INCDIR_INSTALL> \
#                       <PKGDIR>  <PKGDIR_INSTALL>
gcnano_install_lib() {
    # Init for lib dir install
    gcnano_libdir=$1
    gcnano_libdir_install=$2
    # Init for include dir install
    gcnano_incdir=$3
    gcnano_incdir_install=$4
    # Init for package config install
    gcnano_pkgdir=$5
    gcnano_pkgdir_install=$6

    # Install libraries & headers
    install -m 0755 -d ${gcnano_libdir_install}
    install -m 0755 -d ${gcnano_incdir_install}

    # Install libraries
    install -m 0555 ${gcnano_libdir}/libGAL.so ${gcnano_libdir_install}/
    install -m 0555 ${gcnano_libdir}/libVSC.so ${gcnano_libdir_install}/
    # Install includes
    install -m 0755 -d ${gcnano_incdir_install}/KHR
    install -m 0644 ${gcnano_incdir}/KHR/* ${gcnano_incdir_install}/KHR

    # Install compiler libraries when needed
    needs_clc="false"
    needs_glslc="false"
    needs_spirv="false"

    for lib in ${GCNANO_PACKAGECONFIG}; do
        case ${lib} in
            "cl")
                # Install libraries
                find ${gcnano_libdir}/* -type f -name libOpenCL*.so.* -exec install -m 0555 '{}' ${gcnano_libdir_install}/ \;
                # Request CLC and SPIR-V
                needs_clc="true"
                needs_spirv="true"
                ;;
            "egl")
                # Install libraries
                find ${gcnano_libdir}/* -type f -name libEGL.so.* -exec install -m 0555 '{}' ${gcnano_libdir_install}/ \;
                # Install includes
                install -m 0755 -d ${gcnano_incdir_install}/EGL
                install -m 0644 ${gcnano_incdir}/EGL/* ${gcnano_incdir_install}/EGL
                # Install pkgconfig
                install -m 0755 -d ${gcnano_pkgdir_install}
                install -m 0644 ${gcnano_pkgdir}/egl.pc ${gcnano_pkgdir_install}/
                ;;
            "gbm")
                # Install libraries
                find ${gcnano_libdir}/* -type f -name libgbm.so.* -exec install -m 0555 '{}' ${gcnano_libdir_install}/ \;
                install -m 0755 ${gcnano_libdir}/libgbm_viv.so ${gcnano_libdir_install}/
                # Install includes
                install -m 0755 -d ${gcnano_incdir_install}
                install -m 0644 ${gcnano_incdir}/gbm.h ${gcnano_incdir_install}/
                # Install pkgconfig
                install -m 0755 -d ${gcnano_pkgdir_install}
                install -m 644 ${gcnano_pkgdir}/gbm.pc ${gcnano_pkgdir_install}/
                ;;
            "glesv1")
                # Install libraries
                find ${gcnano_libdir}/* -type f -name libGLESv1_CM.so.* -exec install -m 0555 '{}' ${gcnano_libdir_install}/ \;
                # Install includes
                install -m 0755 -d ${gcnano_incdir_install}/GLES
                install -m 0644 ${gcnano_incdir}/GLES/* ${gcnano_incdir_install}/GLES
                # Install pkgconfig
                install -m 0755 -d ${gcnano_pkgdir_install}
                install -m 644 ${gcnano_pkgdir}/glesv1_cm.pc ${gcnano_pkgdir_install}/
                # Request GLSLC
                needs_glslc="true"
                ;;
            "glesv2")
                # Install libraries
                find ${gcnano_libdir}/* -type f -name libGLESv2.so.* -exec install -m 0555 '{}' ${gcnano_libdir_install}/ \;
                # Install includes GLESv2 and GLESv3
                install -m 0755 -d ${gcnano_incdir_install}/GLES2
                install -m 0644 ${gcnano_incdir}/GLES2/* ${gcnano_incdir_install}/GLES2
                # Hack for weston & kmscube compilation
                # Weston and kmscube use GLES3 header but the decision to use gles2 or gles3
                # are made a runtime
                install -m 0755 -d ${gcnano_incdir_install}/GLES3
                install -m 0644 ${gcnano_incdir}/GLES3/* ${gcnano_incdir_install}/GLES3
                # Install pkgconfig
                install -m 0755 -d ${gcnano_pkgdir_install}
                install -m 644 ${gcnano_pkgdir}/glesv2.pc ${gcnano_pkgdir_install}/
                # Request GLSLC
                needs_glslc="true"
                ;;
            "vulkan")
                # Install libraries
                find ${gcnano_libdir}/* -type f -name libvulkan*.so* -exec install -m 0555 '{}' ${gcnano_libdir_install}/ \;
                # Request GLSLC and SPIR-V
                needs_glslc="true"
                needs_spirv="true"
                ;;
            "vg")
                # Install libraries
                find ${gcnano_libdir}/* -type f -name libOpenVG*.so.* -exec install -m 0555 '{}' ${gcnano_libdir_install}/ \;
                # Install includes
                install -m 0755 -d ${gcnano_incdir_install}/VG
                install -m 0644 ${gcnano_incdir}/VG/* ${gcnano_incdir_install}/VG
                # Install pkgconfig
                install -m 0755 -d ${gcnano_pkgdir_install}
                install -m 644 ${gcnano_pkgdir}/vg.pc ${gcnano_pkgdir_install}/
                # Request GLSLC
                needs_glslc="true"
                ;;
            "vx")
                # Install libraries
                find ${gcnano_libdir}/* -type f -name libOpenVX*.so* -exec install -m 0555 '{}' ${gcnano_libdir_install}/ \;
                install -m 0555 ${gcnano_libdir}/libArchModelSw.so ${gcnano_libdir_install}/
                install -m 0555 ${gcnano_libdir}/libNNArchPerf.so ${gcnano_libdir_install}/
                install -m 0555 ${gcnano_libdir}/libovxlib.so ${gcnano_libdir_install}/
                install -m 0555 ${gcnano_libdir}/libNNGPUBinary.so ${gcnano_libdir_install}/
                install -m 0555 ${gcnano_libdir}/libNNVXCBinary.so ${gcnano_libdir_install}/
                install -m 0555 ${gcnano_libdir}/libOvx12VXCBinary.so ${gcnano_libdir_install}/
                install -m 0555 ${gcnano_libdir}/libOvxGPUVXCBinary.so ${gcnano_libdir_install}/
                # Install includes
                install -m 0755 -d ${gcnano_incdir_install}/VX
                install -m 0644 ${gcnano_incdir}/VX/* ${gcnano_incdir_install}/VX
                # Request CLC
                needs_clc="true"
                # To allow building OpenVX kernel program from OpenCL kernel source code,
                # cl_viv_vx_ext.h must be installed at runtime
                install -m 0755 -d ${gcnano_incdir_install}/CL
                install -m 644 ${gcnano_incdir}/CL/cl_viv_vx_ext.h ${gcnano_incdir_install}/CL
                ;;
        esac
    done

    # Install libCLC
    if [ "${needs_clc}" = "true" ]; then
        find ${gcnano_libdir}/* -type f -name libCLC.so -exec install -m 0555 '{}' ${gcnano_libdir_install}/ \;
    fi

    # Install libGLSLC
    if [ "${needs_glslc}" = "true" ]; then
        find ${gcnano_libdir}/* -type f -name libGLSLC.so -exec install -m 0555 '{}' ${gcnano_libdir_install}/ \;
    fi

    # Install libSPIRV
    if [ "${needs_spirv}" = "true" ]; then
        find ${gcnano_libdir}/* -type f -name libSPIRV_viv.so -exec install -m 0555 '{}' ${gcnano_libdir_install}/ \;
    fi
}

# Install utility for gcano-userland:
#   gcnano_install_symlinks  <LIBDIR>  <LIBDIR_INSTALL>
#
# Iterate other installed libraries and create appropriate symlinks if they
# don't exist as symlink or real library at that location.
# For well-formed library with ABI suffix, create a SONAME-suffixed symlink
# and dev-.so-only symlink.
# For .so-only installed library, only create dev-.so-only symlink.
gcnano_install_symlinks() {
    # Init for lib dir install
    gcnano_libdir=$1
    gcnano_lib_symlink_dir_install=$2

    bbnote "Create symlinks to shared libs in ${gcnano_lib_symlink_dir_install} folder (mandatory for SDK)"
    # Init relative path for symlinks
    relative_path=$(realpath -m --relative-to=${gcnano_lib_symlink_dir_install} ${gcnano_libdir})
    # Move to gcnano_lib_symlink_dir_install for symlinks
    cd "${gcnano_lib_symlink_dir_install}"
    for sharelib in $(find ${gcnano_libdir}/ -type f -name "*.so*"); do
        # Get full libname
        sharelib_name=$(basename ${sharelib})
        # Get base libname
        dev_lib_name=$(echo ${sharelib_name} | awk -F'.so' '{print $1}').so
        # Get SONAME
        soname_lib_name=$(objdump -x ${sharelib} 2>/dev/null | grep SONAME | awk -F' ' '{print $NF}')
        # Create symlink
        for link_name in ${soname_lib_name} ${dev_lib_name}; do
            if [ -e "${link_name}" ]; then
                bbnote "Already existing share lib: ${link_name}. No symlink created in ${gcnano_lib_symlink_dir_install}."
            elif [ ! -e "${relative_path}/${sharelib_name}" ]; then
                bbnote "Not able to find share lib: ${relative_path}/${sharelib_name}. No symlink created in ${gcnano_lib_symlink_dir_install}."
            else
                ln -sf ${relative_path}/${sharelib_name} ${link_name}
            fi
        done
    done
    cd "${PWD}"
}

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SRC_URI:append = " ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'vulkan', 'file://vulkan_icd.json.template', '', d)}"
SRC_URI:append = " ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'vx', 'file://openvx_profile.sh.template', '', d)}"

do_install() {
    # Update pkgconfig files info
    for pkg_file in $(find ${B}/pkgconfig/* -type f -name *.pc); do
        sed -e "s;#PREFIX#;${PKGCONFIG_PREFIX};g" -e "s;#VERSION#;${PKGCONFIG_DV};g" -i ${pkg_file}
    done

    # Install gcnano libraries
    gcnano_install_lib  "${B}/${GCNANO_FLAVOUR}/drivers"    "${D}${GCNANO_USERLAND_OUTPUT_LIBDIR}"  \
                        "${B}/${GCNANO_FLAVOUR}/include"    "${D}${includedir}"                     \
                        "${B}/pkgconfig"                    "${D}${libdir}/pkgconfig"

    # Manage creation of additional development symlinks to solve missing libs issue on SDK side
    gcnano_install_symlinks "${D}${GCNANO_USERLAND_OUTPUT_LIBDIR}" "${D}${libdir}"

    if [ -s "${B}/${GCNANO_USERLAND_LDCONF}" ]; then
        install -d ${D}${sysconfdir}/ld.so.conf.d/
        install -m 0644 "${B}/${GCNANO_USERLAND_LDCONF}" ${D}${sysconfdir}/ld.so.conf.d/
    fi

    # Generate OpenCL ICD
    if [ -n "$(echo ${GCNANO_PACKAGECONFIG} | grep 'cl')" ]; then
        install -m 0755 -d "${D}${sysconfdir}/OpenCL/vendors"
        find "${D}${GCNANO_USERLAND_OUTPUT_LIBDIR}" -type f -name "libOpenCL*.so*" | sed s"%${D}%%" > "${D}${sysconfdir}/OpenCL/vendors/${GCNANO_OPENCL_ICD}"
    fi

    # Generate Vulkan ICD
    if [ -n "$(echo ${GCNANO_PACKAGECONFIG} | grep 'vulkan')" ]; then
        vklib=$(find "${D}${GCNANO_USERLAND_OUTPUT_LIBDIR}" -type f -name "libvulkan*.so*" | sed s"%${D}%%")
        vkvers=$(echo ${vklib} | awk -F'.so.' '{print $2}')

        install -m 0755 -d "${D}${sysconfdir}/vulkan/icd.d"
        sed -e "s#%VULKANLIB%#${vklib}#" -e "s#%VULKANVERSION%#${vkvers}#" ${WORKDIR}/vulkan_icd.json.template > ${D}${sysconfdir}/vulkan/icd.d/${GCNANO_VULKAN_ICD}
    fi

    # Install profile script to export required environment variables to allow OpenVX kernel program
    # to be built at runtime
    if [ -n "$(echo ${GCNANO_PACKAGECONFIG} | grep 'vx')" ]; then
        install -m 0755 -d "${D}${sysconfdir}/profile.d"
        sed "s#%VIVANTE_SDK_DIR%#${exec_prefix}#" ${WORKDIR}/openvx_profile.sh.template > ${D}${sysconfdir}/profile.d/openvx_profile.sh
    fi
}

SYSROOT_DIRS:append = " ${GCNANO_USERLAND_OUTPUT_LIBDIR}"

# For the packages that make up the OpenGL interfaces, inject variables so that
# they don't get Debian-renamed (which would remove the -gcnano suffix), and
# RPROVIDEs/RCONFLICTs on the generic libgl name.
python __anonymous() {
    pkgconfig = (d.getVar('GCNANO_PACKAGECONFIG') or "").split()
    for p in (("gbm", "libgbm",),
              ("egl", "libegl", "libegl1"),
              ("glesv1", "libgles1", "libglesv1-cm1"),
              ("glesv2", "libgles2", "libglesv2-2"),
              ("glesv2", "libgles3"),
              ("vg", "libopenvg"),
              ("vx", "libopenvx"),
              ("cl", "libopencl"),
              ("vulkan", "libvulkan-driver")):
        if not p[0] in pkgconfig:
            continue

        pkgs = d.getVar('PACKAGES').split()

        fullp = p[1] + "-gcnano"
        if fullp in pkgs:
            rpkgs = " ".join(p[1:])
            d.setVar("DEBIAN_NOAUTONAME:" + fullp, "1")
            d.appendVar("RREPLACES:" + fullp, rpkgs)
            d.appendVar("RPROVIDES:" + fullp, rpkgs)
            d.appendVar("RCONFLICTS:" + fullp, rpkgs)
            if d.getVar("GCNANO_USERLAND_OUTPUT_LIBDIR") != d.getVar("libdir"):
                d.appendVar("RDEPENDS:" + fullp, " gcnano-ldconf")

        # For -dev, the first element is both the Debian and original name
        fullp += "-dev"
        if fullp in pkgs:
            rpkgs = " ".join([f"{q}-dev" for q in p[1:]])
            d.setVar("DEBIAN_NOAUTONAME:" + fullp, "1")
            d.appendVar("RREPLACES:" + fullp, rpkgs)
            d.appendVar("RPROVIDES:" + fullp, rpkgs)
            d.appendVar("RCONFLICTS:" + fullp, rpkgs)
}

# EGL, GLESv1, GLESv2 and VG run-time packages embed lib*.so to allow
# GPU stack to run properly
# NOTE: it's mandatory to libEGL.so, libGLESv2.so and libOpenVG.so availabale on filesystem
# because gcnano userland library try to make a dlopen on this library (with specific .so extension)
FILES:${PN} = ""
FILES:gcnano-ldconf = "${sysconfdir}/ld.so.conf.d/*"
FILES:libgal-gcnano = "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libGAL.so"
FILES:libgal-gcnano += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libVSC.so"
FILES:libegl-gcnano = "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libEGL.so.* ${libdir}/libEGL.so*"
FILES:libgbm-gcnano = "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libgbm.so.* ${libdir}/libgbm.so.*"
FILES:libgbm-gcnano += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libgbm_viv.so"
FILES:libgles1-gcnano = "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libGLESv1_CM.so.* ${libdir}/libGLESv1_CM.so*"
FILES:libgles2-gcnano = "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libGLESv2.so.* ${libdir}/libGLESv2.so*"
FILES:libopenvg-gcnano = "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libOpenVG.so.* ${libdir}/libOpenVG.so*"
FILES:libopenvx-gcnano = "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libOpenVX*.so* ${libdir}/libOpenVX.so.*"
FILES:libopenvx-gcnano += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libArchModelSw.so"
FILES:libopenvx-gcnano += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libNNArchPerf.so"
FILES:libopenvx-gcnano += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libovxlib.so"
FILES:libopenvx-gcnano += "${includedir}/CL/cl_viv_vx_ext.h ${sysconfdir}/profile.d/*"
FILES:libovxkernels-gcnano += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libNNGPUBinary.so"
FILES:libovxkernels-gcnano += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libNNVXCBinary.so"
FILES:libovxkernels-gcnano += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libOvx12VXCBinary.so"
FILES:libovxkernels-gcnano += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libOvxGPUVXCBinary.so"
FILES:libopencl-gcnano = "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libOpenCL_VSI.so.* ${libdir}/libOpenCL_VSI.so.*"
FILES:libopencl-gcnano += "${sysconfdir}/OpenCL/vendors/*"
FILES:libvulkan-driver-gcnano = "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libvulkan_VSI.so.* ${libdir}/libvulkan_VSI.so.*"
FILES:libvulkan-driver-gcnano += "${sysconfdir}/vulkan/icd.d/*"
FILES:libglslc-gcnano = "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libGLSLC.so"
FILES:libclc-gcnano = "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libCLC.so"
FILES:libspirv-gcnano = "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libSPIRV_viv.so"

FILES:${PN}-dev = ""
FILES:libgal-gcnano-dev = "${libdir}/libGAL.so ${libdir}/libVSC.so"
FILES:libegl-gcnano-dev = "${includedir}/EGL ${includedir}/KHR ${libdir}/pkgconfig/egl.pc"
FILES:libgbm-gcnano-dev = "${libdir}/libgbm.so ${libdir}/libgbm_viv.so ${includedir}/gbm.h ${libdir}/pkgconfig/gbm.pc"
FILES:libgles1-gcnano-dev = "${includedir}/GLES ${libdir}/pkgconfig/glesv1_cm.pc"
FILES:libgles2-gcnano-dev = "${includedir}/GLES2 ${libdir}/pkgconfig/glesv2.pc"
FILES:libopenvg-gcnano-dev = "${includedir}/VG ${libdir}/pkgconfig/vg.pc"
FILES:libgles3-gcnano-dev = "${includedir}/GLES3"
FILES:libopenvg-gcnano-dev = "${includedir}/VG ${libdir}/pkgconfig/vg.pc"
FILES:libopenvx-gcnano-dev = "${libdir}/libOpenVX.so ${libdir}/libOpenVXU.so ${includedir}/VX"
FILES:libopenvx-gcnano-dev += "${libdir}/libArchModelSw.so"
FILES:libopenvx-gcnano-dev += "${libdir}/libNNArchPerf.so"
FILES:libopenvx-gcnano-dev += "${libdir}/libovxlib.so"
FILES:libovxkernels-gcnano-dev += "${libdir}/libNNGPUBinary.so"
FILES:libovxkernels-gcnano-dev += "${libdir}/libNNVXCBinary.so"
FILES:libovxkernels-gcnano-dev += "${libdir}/libOvx12VXCBinary.so"
FILES:libovxkernels-gcnano-dev += "${libdir}/libOvxGPUVXCBinary.so"
FILES:libopencl-gcnano-dev = "${libdir}/libOpenCL_VSI.so"
FILES:libvulkan-driver-gcnano-dev = "${libdir}/libvulkan_VSI.so"
FILES:libglslc-gcnano-dev = "${libdir}/libGLSLC.so"
FILES:libclc-gcnano-dev += "${libdir}/libCLC.so"
FILES:libspirv-gcnano-dev = "${libdir}/libSPIRV_viv.so"

# Set gcnano-userland package with runtime dependencies on all packages
RDEPENDS:${PN} += "\
    libgal-gcnano \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'egl', 'libegl-gcnano', '', d)}       \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'gbm', 'libgbm-gcnano', '', d)}       \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'glesv1', 'libgles1-gcnano', '', d)}  \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'glesv2', 'libgles2-gcnano', '', d)}  \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'vg', 'libopenvg-gcnano', '', d)}     \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'cl', 'libopencl-gcnano', '', d)}     \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'vulkan', 'libvulkan-driver-gcnano', '', d)} \
    "

RDEPENDS:libgles1-gcnano += "libglslc-gcnano"
RDEPENDS:libgles2-gcnano += "libglslc-gcnano"
RDEPENDS:libopenvg-gcnano += "libglslc-gcnano"

RDEPENDS:libopencl-gcnano += "libclc-gcnano libspirv-gcnano"
RDEPENDS:libopenvx-gcnano += "libclc-gcnano libovxkernels-gcnano"
RDEPENDS:libvulkan-driver-gcnano += "libglslc-gcnano libspirv-gcnano"

RDEPENDS:libgles2-gcnano-dev += "libgles3-gcnano-dev"

# Generate empty gcnano-userland package to install all dependencies
ALLOW_EMPTY:${PN} = "1"

# Avoid QA Issue: No GNU_HASH in the elf binary
INSANE_SKIP:libgal-gcnano += "ldflags"
INSANE_SKIP:libegl-gcnano += "ldflags"
INSANE_SKIP:libgbm-gcnano += "ldflags"
INSANE_SKIP:libgles1-gcnano += "ldflags"
INSANE_SKIP:libgles2-gcnano += "ldflags"
INSANE_SKIP:libopenvg-gcnano += "ldflags"
INSANE_SKIP:libopenvx-gcnano += "ldflags"
INSANE_SKIP:libovxkernels-gcnano += "ldflags"
INSANE_SKIP:libclc-gcnano += "ldflags"
INSANE_SKIP:libopencl-gcnano += "ldflags"
INSANE_SKIP:libvulkan-driver-gcnano += "ldflags"
INSANE_SKIP:libglslc-gcnano += "ldflags"
INSANE_SKIP:libspirv-gcnano += "ldflags"

# Avoid QA Issue: non -dev/-dbg/nativesdk- package contains symlink .so
INSANE_SKIP:libegl-gcnano += "dev-so"
INSANE_SKIP:libgles1-gcnano += "dev-so"
INSANE_SKIP:libgles2-gcnano += "dev-so"
INSANE_SKIP:libopenvg-gcnano += "dev-so"
INSANE_SKIP:libopenvx-gcnano += "dev-so"
