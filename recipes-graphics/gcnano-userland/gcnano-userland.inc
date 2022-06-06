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
PKGCONFIG_DV ??= "21.1.1"
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

    # Install libraries
    install -m 0755 -d ${gcnano_libdir_install}

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
                find ${gcnano_libdir}/* -type f -name libEGL.so* -exec install -m 0555 '{}' ${gcnano_libdir_install}/ \;
                find ${gcnano_libdir}/* -type l -name libEGL.so* -exec cp -d '{}' ${gcnano_libdir_install}/ \;
                # Install includes
                install -m 0755 -d ${gcnano_incdir_install}/EGL
                install -m 0644 ${gcnano_incdir}/EGL/* ${gcnano_incdir_install}/EGL
                # Install pkgconfig
                install -m 0755 -d ${gcnano_pkgdir_install}
                install -m 0644 ${gcnano_pkgdir}/egl.pc ${gcnano_pkgdir_install}/
                ;;
            "gbm")
                # Install libraries
                find ${gcnano_libdir}/* -type f -name libgbm.so* -exec install -m 0555 '{}' ${gcnano_libdir_install}/ \;
                find ${gcnano_libdir}/* -type l -name libgbm.so* -exec cp -d '{}' ${gcnano_libdir_install}/ \;
                install -m 0755 ${gcnano_libdir}/libgbm_viv.so ${gcnano_libdir_install}/
                # Install includes
                install -m 0755 -d ${gcnano_incdir_install}
                if [ -e "${gcnano_incdir}/gbm/gbm.h" ]; then
                    install -m 0644 ${gcnano_incdir}/gbm/gbm.h ${gcnano_incdir_install}/
                else
                    install -m 0644 ${gcnano_incdir}/gbm.h ${gcnano_incdir_install}/
                fi
                # Install pkgconfig
                install -m 0755 -d ${gcnano_pkgdir_install}
                install -m 644 ${gcnano_pkgdir}/gbm.pc ${gcnano_pkgdir_install}/
                ;;
            "glesv1")
                # Install libraries
                install -m 0555 ${gcnano_libdir}/libGLESv1_CM.so ${gcnano_libdir_install}/
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
                find ${gcnano_libdir}/* -type f -name libGLESv2.so* -exec install -m 0555 '{}' ${gcnano_libdir_install}/ \;
                find ${gcnano_libdir}/* -type l -name libGLESv2.so* -exec cp -d '{}' ${gcnano_libdir_install}/ \;
                # Install includes
                install -m 0755 -d ${gcnano_incdir_install}/GLES2
                install -m 0644 ${gcnano_incdir}/GLES2/* ${gcnano_incdir_install}/GLES2
                # Install pkgconfig
                install -m 0755 -d ${gcnano_pkgdir_install}
                install -m 644 ${gcnano_pkgdir}/glesv2.pc ${gcnano_pkgdir_install}/
                ;;
            "vg")
                # Install libraries
                find ${gcnano_libdir}/* -type f -name libOpenVG*.so* -exec install -m 0555 '{}' ${gcnano_libdir_install}/ \;
                find ${gcnano_libdir}/* -type l -name libOpenVG*.so* -exec cp -d '{}' ${gcnano_libdir_install}/ \;
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

do_install() {
    # Update pkgconfig files info
    for pkg_file in $(find ${B}/pkgconfig/* -type f -name *.pc); do
        sed -e "s;#PREFIX#;${PKGCONFIG_PREFIX};g" -e "s;#VERSION#;${PKGCONFIG_DV};g" -i ${pkg_file}
    done

    # Install gcnano libraries
    gcnano_install_lib  "${B}/${GCNANO_FLAVOUR}/drivers"    "${D}${GCNANO_USERLAND_OUTPUT_LIBDIR}"  \
                        "${B}/${GCNANO_FLAVOUR}/include"    "${D}${includedir}"                     \
                        "${B}/pkgconfig"                    "${D}${libdir}/pkgconfig"

    if [ -s "${B}/${GCNANO_USERLAND_LDCONF}" ]; then
        install -d ${D}${sysconfdir}/ld.so.conf.d/
        install -m 0644 "${B}/${GCNANO_USERLAND_LDCONF}" ${D}${sysconfdir}/ld.so.conf.d/
    fi
}

# Manage install for gcnano-userland libs into specific folder for sysroot as well
gcnano_sysroot() {
    if [ "${GCNANO_USERLAND_OUTPUT_LIBDIR}" != "${libdir}" ]; then
        install -d ${SYSROOT_DESTDIR}${libdir}
        cp -aR ${D}${GCNANO_USERLAND_OUTPUT_LIBDIR}/* ${SYSROOT_DESTDIR}${libdir}/
    fi
}
SYSROOT_PREPROCESS_FUNCS =+ "gcnano_sysroot"

# For the packages that make up the OpenGL interfaces, inject variables so that
# they don't get Debian-renamed (which would remove the -gcnano suffix), and
# RPROVIDEs/RCONFLICTs on the generic libgl name.
python __anonymous() {
    pkgconfig = (d.getVar('GCNANO_PACKAGECONFIG') or "").split()
    for p in (("gbm", "libgbm", "libgbm"),
              ("egl", "libegl", "libegl1"),
              ("gles", "libgles1", "libglesv1-cm1"),
              ("gles", "libgles2", "libglesv2-2"),
              ("vg", "libopenvg", "libopenvg")):
        if not p[0] in pkgconfig:
            continue
        fullp = p[1] + "-gcnano"
        pkgs = " ".join(p[1:])
        d.setVar("DEBIAN_NOAUTONAME_" + fullp, "1")
        d.appendVar("RREPLACES:" + fullp, pkgs)
        d.appendVar("RPROVIDES:" + fullp, pkgs)
        d.appendVar("RCONFLICTS:" + fullp, pkgs)

        if d.getVar("GCNANO_USERLAND_OUTPUT_LIBDIR") != d.getVar("libdir"):
            d.appendVar("RDEPENDS:" + fullp, " gcnano-ldconf")

        # For -dev, the first element is both the Debian and original name
        fullp += "-dev"
        pkgs = p[1] + "-dev"
        d.setVar("DEBIAN_NOAUTONAME_" + fullp, "1")
        d.appendVar("RREPLACES:" + fullp, pkgs)
        d.appendVar("RPROVIDES:" + fullp, pkgs)
        d.appendVar("RCONFLICTS:" + fullp, pkgs)
}

FILES:${PN} = ""
FILES:gcnano-ldconf = "${sysconfdir}/"
FILES:libgal-gcnano = "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libGAL.so"
FILES:libgal-gcnano += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libVSC.so"
FILES:libegl-gcnano = "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libEGL.so*"
FILES:libgbm-gcnano = "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libgbm.so*"
FILES:libgbm-gcnano += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libgbm_viv.so"
FILES:libgles1-gcnano = "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libGLESv1*.so"
FILES:libgles2-gcnano = "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libGLESv2.so"
FILES:libgles2-gcnano += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libGLSLC.so"
FILES:libopenvg-gcnano = "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libOpenVG*.so*"

FILES:${PN}-dev = ""
FILES:libegl-gcnano-dev = "${includedir}/EGL ${includedir}/KHR ${libdir}/pkgconfig/egl.pc"
FILES:libgbm-gcnano-dev = "${includedir}/gbm.h ${libdir}/pkgconfig/gbm.pc"
FILES:libgles1-gcnano-dev = "${includedir}/GLES ${libdir}/pkgconfig/glesv1*.pc"
FILES:libgles2-gcnano-dev = "${includedir}/GLES2 ${libdir}/pkgconfig/glesv2.pc"
FILES:libopenvg-gcnano-dev = "${includedir}/VG ${libdir}/pkgconfig/vg.pc"

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
INSANE_SKIP:libegl-gcnano += "dev-so"
INSANE_SKIP:libgbm-gcnano += "dev-so"
INSANE_SKIP:libopenvg-gcnano += "dev-so"
