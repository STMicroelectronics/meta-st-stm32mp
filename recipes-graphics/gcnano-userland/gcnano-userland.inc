PACKAGE_ARCH = "${MACHINE_ARCH}"

GCNANO_PACKAGECONFIG ??= ""

PROVIDES = "\
    gcnano-userland \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'egl', 'virtual/egl', '', d)}          \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'gbm', 'virtual/libgbm', '', d)}       \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'glesv1', 'virtual/libgles1', '', d)}  \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'glesv2', 'virtual/libgles2', '', d)}  \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'vg', 'virtual/libopenvg', '', d)}     \
    "

PACKAGES += "\
    gcnano-ldconf                                                                                           \
    libgal-gcnano libgal-gcnano-dev                                                                         \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'egl', 'libegl-gcnano libegl-gcnano-dev', '', d)}          \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'gbm', 'libgbm-gcnano libgbm-gcnano-dev', '', d)}          \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'glesv1', 'libgles1-gcnano libgles1-gcnano-dev', '', d)}   \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'glesv2', 'libgles2-gcnano libgles2-gcnano-dev', '', d)}   \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'vg', 'libopenvg-gcnano libopenvg-gcnano-dev', '', d)}     \
    "

GCNANO_BACKEND ??= "multi"
GCNANO_VERSION ??= "6"
GCNANO_DATE ??= ""

# Init the gcnano library version
GCNANO_LIBVERSION ?= "${GCNANO_VERSION}.${GCNANO_BACKEND}"

# Init the gcnano build type
GCNANO_FLAVOUR ??= "release"

# Init the gcnano tarball file name
GCNANO_TAR_FILENAME ?= "gcnano-userland-${GCNANO_BACKEND}-${GCNANO_VERSION}-${GCNANO_DATE}"

# Configure gcnano output library dir
GCNANO_USERLAND_OUTPUT_LIBDIR ??= "${libdir}"

GCNANO_USERLAND_LDCONF ?= "gcnano.conf"

# Configure the pkgconfig settings
PKGCONFIG_DV ??= "22.0.3"
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

    for lib in ${GCNANO_PACKAGECONFIG}; do
        case ${lib} in
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
                ;;
            "glesv2")
                # Install libraries
                install -m 0555 ${gcnano_libdir}/libGLSLC.so ${gcnano_libdir_install}/
                find ${gcnano_libdir}/* -type f -name libGLESv2.so.* -exec install -m 0555 '{}' ${gcnano_libdir_install}/ \;
                # Install includes
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
                ;;
        esac
    done
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
    # when GCNANO_USERLAND_OUTPUT_LIBDIR differs from libdir
    gcnano_install_symlinks "${D}${GCNANO_USERLAND_OUTPUT_LIBDIR}" "${D}${libdir}"
    if [ "${GCNANO_USERLAND_OUTPUT_LIBDIR}" != "${libdir}" ]; then
        gcnano_install_symlinks "${D}${GCNANO_USERLAND_OUTPUT_LIBDIR}" "${D}${GCNANO_USERLAND_OUTPUT_LIBDIR}"
    fi

    if [ -s "${B}/${GCNANO_USERLAND_LDCONF}" ]; then
        install -d ${D}${sysconfdir}/ld.so.conf.d/
        install -m 0644 "${B}/${GCNANO_USERLAND_LDCONF}" ${D}${sysconfdir}/ld.so.conf.d/
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
              ("vg", "libopenvg")):
        if not p[0] in pkgconfig:
            continue

        pkgs = d.getVar('PACKAGES').split()

        fullp = p[1] + "-gcnano"
        if fullp in pkgs:
            rpkgs = " ".join(p[1:])
            d.setVar("DEBIAN_NOAUTONAME_" + fullp, "1")
            d.appendVar("RREPLACES:" + fullp, rpkgs)
            d.appendVar("RPROVIDES:" + fullp, rpkgs)
            d.appendVar("RCONFLICTS:" + fullp, rpkgs)
            if d.getVar("GCNANO_USERLAND_OUTPUT_LIBDIR") != d.getVar("libdir"):
                d.appendVar("RDEPENDS:" + fullp, " gcnano-ldconf")

        # For -dev, the first element is both the Debian and original name
        fullp += "-dev"
        if fullp not in pkgs:
            rpkgs = p[1] + "-dev"
            d.setVar("DEBIAN_NOAUTONAME_" + fullp, "1")
            d.appendVar("RREPLACES:" + fullp, rpkgs)
            d.appendVar("RPROVIDES:" + fullp, rpkgs)
            d.appendVar("RCONFLICTS:" + fullp, rpkgs)
}

# EGL, GLESv1, GLESv2 and VG run-time packages embed lib*.so to allow
# GPU stack to run properly
# NOTE: it's mandatory to libEGL.so, libGLESv2.so and libOpenVG.so availabale on filesystem
# because gcnano userland library try to make a dlopen on this library (with specific .so extension)
FILES:${PN} = ""
FILES:gcnano-ldconf = "${sysconfdir}/"
FILES:libgal-gcnano = "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libGAL.so"
FILES:libgal-gcnano += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libVSC.so"
FILES:libegl-gcnano = "${@oe.utils.ifelse(d.getVar('GCNANO_USERLAND_OUTPUT_LIBDIR') != d.getVar('libdir'), '${GCNANO_USERLAND_OUTPUT_LIBDIR}/libEGL.so*','${GCNANO_USERLAND_OUTPUT_LIBDIR}/libEGL.so.*')}"
FILES:libgbm-gcnano = "${@oe.utils.ifelse(d.getVar('GCNANO_USERLAND_OUTPUT_LIBDIR') != d.getVar('libdir'), '${GCNANO_USERLAND_OUTPUT_LIBDIR}/libgbm.so*','${GCNANO_USERLAND_OUTPUT_LIBDIR}/libgbm.so.*')}"
FILES:libgbm-gcnano += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libgbm_viv.so"
FILES:libgles1-gcnano = "${@oe.utils.ifelse(d.getVar('GCNANO_USERLAND_OUTPUT_LIBDIR') != d.getVar('libdir'), '${GCNANO_USERLAND_OUTPUT_LIBDIR}/libGLESv1_CM.so*','${GCNANO_USERLAND_OUTPUT_LIBDIR}/libGLESv1_CM.so.*')}"
FILES:libgles2-gcnano = "${@oe.utils.ifelse(d.getVar('GCNANO_USERLAND_OUTPUT_LIBDIR') != d.getVar('libdir'), '${GCNANO_USERLAND_OUTPUT_LIBDIR}/libGLESv2.so*','${GCNANO_USERLAND_OUTPUT_LIBDIR}/libGLESv2.so.*')}"
FILES:libgles2-gcnano += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libGLSLC.so"
FILES:libopenvg-gcnano = "${@oe.utils.ifelse(d.getVar('GCNANO_USERLAND_OUTPUT_LIBDIR') != d.getVar('libdir'), '${GCNANO_USERLAND_OUTPUT_LIBDIR}/libOpenVG*.so*','${GCNANO_USERLAND_OUTPUT_LIBDIR}/libOpenVG*.so.*')}"

FILES:${PN}-dev = ""
FILES:libgal-gcnano-dev = ""
FILES:libegl-gcnano-dev = "${includedir}/EGL ${includedir}/KHR ${libdir}/pkgconfig/egl.pc"
FILES:libgbm-gcnano-dev = "${includedir}/gbm.h ${libdir}/pkgconfig/gbm.pc"
FILES:libgles1-gcnano-dev = "${includedir}/GLES ${libdir}/pkgconfig/glesv1*.pc"
FILES:libgles2-gcnano-dev = "${includedir}/GLES2 ${includedir}/GLES3 ${libdir}/pkgconfig/glesv2.pc"
FILES:libopenvg-gcnano-dev = "${includedir}/VG ${libdir}/pkgconfig/vg.pc"
# Append additional symlinks to *-dev packages for delivered shared libs
# when GCNANO_USERLAND_OUTPUT_LIBDIR differs from libdir
FILES:libgal-gcnano-dev += "${libdir}/libGAL.so"
FILES:libgal-gcnano-dev += "${libdir}/libVSC.so"
FILES:libegl-gcnano-dev += "${@oe.utils.ifelse(d.getVar('GCNANO_USERLAND_OUTPUT_LIBDIR') != d.getVar('libdir'), '${libdir}/libEGL.so*','${libdir}/libEGL.so')}"
FILES:libgbm-gcnano-dev += "${@oe.utils.ifelse(d.getVar('GCNANO_USERLAND_OUTPUT_LIBDIR') != d.getVar('libdir'), '${libdir}/libgbm.so*','${libdir}/libgbm.so')}"
FILES:libgbm-gcnano-dev += "${libdir}/libgbm_viv.so"
FILES:libgles1-gcnano-dev += "${@oe.utils.ifelse(d.getVar('GCNANO_USERLAND_OUTPUT_LIBDIR') != d.getVar('libdir'), '${libdir}/libGLESv1*.so*','${libdir}/libGLESv1*.so')}"
FILES:libgles2-gcnano-dev += "${@oe.utils.ifelse(d.getVar('GCNANO_USERLAND_OUTPUT_LIBDIR') != d.getVar('libdir'), '${libdir}/libGLESv2.so*','${libdir}/libGLESv2.so')}"
FILES:libgles2-gcnano-dev += "${libdir}/libGLSLC.so"
FILES:libopenvg-gcnano-dev += "${@oe.utils.ifelse(d.getVar('GCNANO_USERLAND_OUTPUT_LIBDIR') != d.getVar('libdir'), '${libdir}/libOpenVG*.so*','${libdir}/libOpenVG*.so')}"

# Set gcnano-userland package with runtime dependencies on all packages
RDEPENDS:${PN} += "\
    libgal-gcnano \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'egl', 'libegl-gcnano', '', d)}       \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'gbm', 'libgbm-gcnano', '', d)}       \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'glesv1', 'libgles1-gcnano', '', d)}  \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'glesv2', 'libgles2-gcnano', '', d)}  \
    ${@bb.utils.contains('GCNANO_PACKAGECONFIG', 'vg', 'libopenvg-gcnano', '', d)}     \
    "

# Generate empty gcnano-userland package to install all dependencies
ALLOW_EMPTY:${PN} = "1"

# Avoid QA Issue: No GNU_HASH in the elf binary
INSANE_SKIP:libgal-gcnano += "ldflags"
INSANE_SKIP:libegl-gcnano += "ldflags"
INSANE_SKIP:libgbm-gcnano += "ldflags"
INSANE_SKIP:libgles1-gcnano += "ldflags"
INSANE_SKIP:libgles2-gcnano += "ldflags"
INSANE_SKIP:libopenvg-gcnano += "ldflags"

# Avoid QA Issue: non -dev/-dbg/nativesdk- package contains symlink .so
INSANE_SKIP:libgles1-gcnano += "dev-so"
INSANE_SKIP:libgles2-gcnano += "dev-so"
INSANE_SKIP:libegl-gcnano += "dev-so"
INSANE_SKIP:libgbm-gcnano += "dev-so"
INSANE_SKIP:libopenvg-gcnano += "dev-so"
