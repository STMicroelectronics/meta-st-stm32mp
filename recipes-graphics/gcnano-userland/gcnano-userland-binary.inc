DESCRIPTION  = "STMicrolectronics port of the EGL, GLESv1_CM and GLES_v2 libraries from Vivante for the gcnano 3D core."
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${TAR_FILENAME}/LICENSE;md5=dd36864f287701862a189a69fc50f1d8"

PACKAGE_ARCH = "${MACHINE_ARCH}"

PROVIDES += "gcnano-userland virtual/libgles1 virtual/libgles2 virtual/egl virtual/libvg virtual/libgbm"

TAR_FILENAME = "gcnano-userland-${BACKEND}-${PV}-${GCNANO_USERLAND_FB_TARBALL_DATE}"

SRC_URI = "git://github.com/STMicroelectronics/gcnano-binaries;protocol=https;branch=gcnano-6.4.3-binaries"
SRCREV = "969d11518529bb70a132909b02ddcc5736c92d91"

PV = "6.4.3"

GCNANO_USERLAND_FB_TARBALL_DATE = "20210726"

S = "${WORKDIR}/git"

# Variable for using vendor directory instead of usr
GCNANO_USERLAND_USE_VENDOR_DIR ?= "1"
GCNANO_USERLAND_VENDOR_DIR ?= "/vendor"
GCNANO_USERLAND_OUTPUT_LIBDIR = "${@'${GCNANO_USERLAND_VENDOR_DIR}/lib' if d.getVar('GCNANO_USERLAND_USE_VENDOR_DIR') == '1' else '${libdir}'}"

# Action stubbed
do_configure[noexec] = "1"
do_compile[noexec] = "1"

#------------------------------------------
# Overwrite of do_unpack to untar the tarball
do_unpack[depends] += "xz-native:do_populate_sysroot"
gcnano_userland_binaries_unpack() {
    cd ${S}
    sh ${TAR_FILENAME}.bin --auto-accept
}

python do_unpack() {
    eula = d.getVar('ACCEPT_EULA_'+d.getVar('MACHINE'))
    eula_file = d.getVar('EULA_FILE_ST')
    machine = d.getVar('MACHINE')
    pkg = d.getVar('PN')
    if eula == None:
        bb.fatal("To use '%s' you need to accept the STMicroelectronics EULA at '%s'. "
                 "Please read it and in case you accept it, write: "
                 "ACCEPT_EULA_%s = \"1\" in your local.conf." % (pkg, eula_file, machine))
    elif eula == '0':
        bb.fatal("To use '%s' you need to accept the STMicroelectronics EULA." % pkg)
    else:
        bb.note("STMicroelectronics EULA has been accepted for '%s'" % pkg)

    try:
        externalsrc = d.getVar('EXTERNALSRC')
        if not externalsrc:
            bb.build.exec_func('base_do_unpack', d)
            bb.build.exec_func('gcnano_userland_binaries_unpack', d)
    except:
        raise
}
do_unpack[vardepsexclude] += "EULA_FILE_ST"

#------------------------------------------
# clean
#
clean_release_file() {
    rm -f ${D}/vendor/lib/*.multi.release.so
    rm -f ${D}${libdir}/*.multi.release.so
}
clean_debug_file() {
    rm -f ${D}/vendor/lib/*.multi.debug.so
    rm -f ${D}${libdir}/*.multi.debug.so
}

#------------------------------------------
# Do install
#
create_link() {
    prefix=$1
    library_link=$2
    library_src_relative_path_to_dst=$3
    library_dst=$4
    LINK=`readlink $prefix/$library_dst/$library_src_relative_path_to_dst/$library_link`
    if [ -L $prefix/$library_dst/$library_src_relative_path_to_dst/$LINK ];
    then
        # point on a link, need to get the path of link
        SUBLINK=$(readlink $prefix/$library_dst/$library_src_relative_path_to_dst/$LINK)
        ln -sf $library_src_relative_path_to_dst/$SUBLINK $prefix/$library_dst/$library_link
    else
        ln -sf $library_src_relative_path_to_dst/$LINK $prefix/$library_dst/$library_link
    fi
}
do_install() {
    if [ ${GCNANO_USERLAND_USE_VENDOR_DIR} -eq 1 ];
    then
        install -m 755 -d ${D}/${GCNANO_USERLAND_OUTPUT_LIBDIR}/ ${D}${libdir}
        cp -R ${S}/${TAR_FILENAME}/usr ${D}/
        mv ${D}${libdir}/*.so* ${D}/${GCNANO_USERLAND_OUTPUT_LIBDIR}/

        cd ${D}${libdir}/
        if [ -e ${D}/${GCNANO_USERLAND_OUTPUT_LIBDIR}/gbm_viv.so ];
        then
            ln -sf ../..${GCNANO_USERLAND_OUTPUT_LIBDIR}/gbm_viv.so ${D}${libdir}/gbm_viv.so
        fi
        if [ -e ${D}/${GCNANO_USERLAND_OUTPUT_LIBDIR}/libgbm_viv.so ];
        then
            ln -sf ../..${GCNANO_USERLAND_OUTPUT_LIBDIR}/libgbm_viv.so ${D}${libdir}/libgbm_viv.so
        fi
        for link in `find ${D}/${GCNANO_USERLAND_OUTPUT_LIBDIR} -type l`;
        do
            library_link=`basename $link`
            create_link ${D} $library_link  ../..${GCNANO_USERLAND_OUTPUT_LIBDIR} ${libdir}
        done

        chown -R root.root ${D}/${GCNANO_USERLAND_OUTPUT_LIBDIR} ${D}${libdir}/
    else
        install -m 755 -d ${D}/usr/
        cp -aR ${S}/${TAR_FILENAME}/usr ${D}/
    fi

    # remove remove libwayland-egl which are now provided by wayland
    if test -f ${D}${GCNANO_USERLAND_OUTPUT_LIBDIR}/libwayland-egl.so ;
    then
        rm ${D}${GCNANO_USERLAND_OUTPUT_LIBDIR}/libwayland-egl.so*
    fi
    if test -L ${D}${libdir}/libwayland-egl.so ;
    then
        rm ${D}${libdir}/libwayland-egl.so*
    fi
    if test -f ${D}${libdir}/pkgconfig/wayland-egl.pc ;
    then
        rm ${D}${libdir}/pkgconfig/wayland-egl.pc
    fi

    chown -R root.root ${D}/${GCNANO_USERLAND_OUTPUT_LIBDIR} ${D}${libdir}/
}

# Cannot split or strip last added firmwares
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_SYSROOT_STRIP = "1"

# Avoid QA Issue: No GNU_HASH in the elf binary
INSANE_SKIP_${PN} += "ldflags"

# Avoid QA Issue: non -dev/-dbg/nativesdk- package contains symlink .so
INSANE_SKIP_${PN} += "dev-so"
# Avoid QA Issue: file-rdeps
INSANE_SKIP_${PN} += "file-rdeps"
# Avoid QA Issue: debug-deps
INSANE_SKIP_${PN} += "debug-deps"

INSANE_SKIP_${PN}-dev += "dev-elf dev-so ldflags file-rdeps"

# Monolitic configuration
RPROVIDES_${PN} = "libegl libegl1 libgles1 libglesv1-cm1 libgles2 libglesv2-2 libvg libgbm gbm_viv"
RREPLACES_${PN} = "libegl libegl1 libgles1 libglesv1-cm1 libgles2 libglesv2-2 libvg libgbm gbm_viv"
RCONFLICTS_${PN} = "libegl libegl1 libgles1 libglesv1-cm1 libgles2 libglesv2-2 libvg libgbm gbm_viv"

PACKAGES = "${PN} ${PN}-dev "

SUMMARY_${PN} = "${SUMMARY}"
FILES_${PN}  = "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libEGL.so* ${GCNANO_USERLAND_OUTPUT_LIBDIR}/libEGL.*.${GCNANO_TYPE}.so"
FILES_${PN} += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libGLESv1_CM.so ${GCNANO_USERLAND_OUTPUT_LIBDIR}/libGLESv1_CM.*.${GCNANO_TYPE}.so"
FILES_${PN} += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libGLESv2.so* ${GCNANO_USERLAND_OUTPUT_LIBDIR}/libGLESv2.*.${GCNANO_TYPE}.so"
FILES_${PN} += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libOpenVG.so ${GCNANO_USERLAND_OUTPUT_LIBDIR}/libOpenVG.*.${GCNANO_TYPE}.so"
FILES_${PN} += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libGAL.so ${GCNANO_USERLAND_OUTPUT_LIBDIR}/libGAL.*.${GCNANO_TYPE}.so"
FILES_${PN} += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libVSC.so ${GCNANO_USERLAND_OUTPUT_LIBDIR}/libVSC.*.${GCNANO_TYPE}.so"
FILES_${PN} += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libGLSLC.so ${GCNANO_USERLAND_OUTPUT_LIBDIR}/libGLSLC.*.${GCNANO_TYPE}.so"
FILES_${PN} += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libgbm.so* ${GCNANO_USERLAND_OUTPUT_LIBDIR}/libgbm.*.${GCNANO_TYPE}.so*"
FILES_${PN} += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libwayland-viv*.so*"
FILES_${PN} += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/gbm_viv.so ${GCNANO_USERLAND_OUTPUT_LIBDIR}/gbm_viv.*.${GCNANO_TYPE}.so"
FILES_${PN} += "${GCNANO_USERLAND_OUTPUT_LIBDIR}/libgbm_viv.so ${GCNANO_USERLAND_OUTPUT_LIBDIR}/libgbm_viv.*.${GCNANO_TYPE}.so"

FILES_${PN} += "${libdir}/lib*.so ${libdir}/lib*${SOLIBS} ${libdir}/gbm*"

SUMMARY_${PN}-dev = "${SUMMARY_${PN}} - Development files"
FILES_${PN}-dev   = "${includedir} ${libdir}/pkgconfig/ ${libdir}/"

gcnano_sysroot() {
    if [ ${GCNANO_USERLAND_USE_VENDOR_DIR} -eq 1 ];
    then
        # put all lthe library for on a same directory
        # clean usr and  ${GCNANO_USERLAND_OUTPUT_LIBDIR}
        rm -rf ${SYSROOT_DESTDIR}/${libdir} ${SYSROOT_DESTDIR}/${GCNANO_USERLAND_OUTPUT_LIBDIR}
        #copy from tarball
        cp -aR ${S}/${TAR_FILENAME}/usr ${SYSROOT_DESTDIR}/
        if test -f ${SYSROOT_DESTDIR}/${libdir}/libwayland-egl.so ;
        then
            rm ${SYSROOT_DESTDIR}${libdir}/libwayland-egl.so*
            rm ${SYSROOT_DESTDIR}${libdir}/pkgconfig/wayland-egl.pc
        fi
    fi
}
SYSROOT_PREPROCESS_FUNCS =+ "gcnano_sysroot"
