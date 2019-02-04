
do_install_append_class-nativesdk() {
	mkdir -p ${D}${SDKPATHNATIVE}/environment-setup.d
	echo 'export BISON_PKGDATADIR="$OECORE_NATIVE_SYSROOT/usr/share/bison"' > ${D}${SDKPATHNATIVE}/environment-setup.d/bison.sh
}

FILES_${PN}_append_class-nativesdk = " ${SDKPATHNATIVE}/environment-setup.d/bison.sh"
