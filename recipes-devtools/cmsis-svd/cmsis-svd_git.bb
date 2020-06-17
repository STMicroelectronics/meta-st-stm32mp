SUMMARY = "CMSIS SVD data files and parser"

LICENSE = " Apache-2.0 & svd-Atmel & svd-Freescale & svd-Fujitsu & svd-STMicro "

LIC_FILES_CHKSUM = "\
    file://LICENSE-APACHE;md5=fa818a259cbed7ce8bc2a22d35a464fc \
    file://data/STMicro/License.html;md5=9a2821012ac32bea060eccbc76512bdb \
    file://data/Freescale/Freescale%20CMSIS-SVD%20License%20Agreement.pdf;md5=33928757d8c2861dc9256ce344d11db3 \
    file://data/Fujitsu/License.html;md5=e630487a365e7e0c5e03afcc644ce0ad \
    file://data/Atmel/License.html;md5=466a7215aa18f98886ba2c15dba6b35a \
    "

NO_GENERIC_LICENSE[svd-Atmel] = "data/Atmel/License.html"
NO_GENERIC_LICENSE[svd-Freescale] = "data/Freescale/Freescale CMSIS-SVD License Agreement.pdf"
NO_GENERIC_LICENSE[svd-Fujitsu] = "data/Fujitsu/License.html"
NO_GENERIC_LICENSE[svd-STMicro] = "data/STMicro/License.html"

inherit pkgconfig autotools-brokensep gettext

SRC_URI = "git://github.com/posborne/cmsis-svd.git;protocol=https"
SRCREV = "2ab163c2aea83eb9b39c163856450089255ce4f2"

PV = "0.4+git${SRCPV}"

S = "${WORKDIR}/git"

BBCLASSEXTEND += "native nativesdk"

PACKAGES += "\
    ${PN}-parser            \
    \
    ${PN}-apache-license    \
    \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'Atmel', '${PN}-data-atmel ${PN}-atmel-license', '', d)}               \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'Freescale', '${PN}-data-freescale ${PN}-freescale-license', '', d)}   \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'Fujitsu', '${PN}-data-fujitsu ${PN}-fujitsu-license', '', d)}         \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'Holtek', '${PN}-data-holtek', '', d)}                                 \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'Nordic', '${PN}-data-nordic', '', d)}                                 \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'Nuvoton', '${PN}-data-nuvoton', '', d)}                               \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'NXP', '${PN}-data-nxp', '', d)}                                       \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'SiliconLabs', '${PN}-data-siliconlabs', '', d)}                       \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'Spansion', '${PN}-data-spansion', '', d)}                             \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'STMicro', '${PN}-data-stmicro ${PN}-stmicro-license', '', d)}         \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'TexasInstruments', '${PN}-data-texasinstruments', '', d)}             \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'Toshiba', '${PN}-data-toshiba', '', d)}                               \
    "

CMSIS_SVD_DATA ?= "\
    STMicro \
    "

# Empty cmsis-svd packages to use it as a meta package for dependencies install
FILES_${PN}_class-target = ""

do_configure[noexec] = "1"
do_compile[noexec] = "1"

INSTALL_PATH = "${datadir}/cmsis-svd/cmsis_svd"

do_install () {
    install -d ${D}${INSTALL_PATH}
    install -m 0644 ${S}/LICENSE-APACHE ${D}${INSTALL_PATH}
    install -m 0755 ${S}/python/cmsis_svd/*.py ${D}${INSTALL_PATH}

    install -d ${D}${INSTALL_PATH}/examples
    cp -R ${S}/python/cmsis_svd/examples/* ${D}${INSTALL_PATH}/examples

    install -d ${D}${INSTALL_PATH}/tests
    cp -R ${S}/python/cmsis_svd/tests/* ${D}${INSTALL_PATH}/tests

    install -d ${D}${INSTALL_PATH}/data

    # Filter requested data files
    for data_path in ${CMSIS_SVD_DATA}
    do
        if [ -d "${S}/data/${data_path}" ]; then
            cp -R ${S}/data/${data_path} ${D}${INSTALL_PATH}/data
        else
            bbwarn "Can not find ${data_path} in ${S}/data"
        fi
    done
    # Remove unexpected 'Contents.txt' files
    find ${D}${INSTALL_PATH}/data -type f -name Contents.txt -exec rm -f {} \;
}

# For parser and ARM_SAMPLE svd file example
LICENSE_${PN}-parser = "Apache-2.0"
LICENSE_${PN}-apache-license = "Apache-2.0"

FILES_${PN}-apache-license = "${INSTALL_PATH}/LICENSE-APACHE"
FILES_${PN}-parser = "${INSTALL_PATH}/*.py ${INSTALL_PATH}/examples ${INSTALL_PATH}/tests ${INSTALL_PATH}/data/ARM_SAMPLE/*.svd"

RDEPENDS_${PN}-parser += "${PN}-apache-license"
# For python dependencies
RDEPENDS_${PN}-parser += "python3-json"
RDEPENDS_${PN}-parser += "python3-setuptools"
RDEPENDS_${PN}-parser += "python3-six"
RDEPENDS_${PN}-parser += "python3-xml"

# For Atmel
LICENSE_${PN}-data-atmel = "svd-Atmel"
LICENSE_${PN}-atmel-license = "svd-Atmel"

FILES_${PN}-atmel-license = "${INSTALL_PATH}/data/Atmel/License.html"
FILES_${PN}-data-atmel = "${INSTALL_PATH}/data/Atmel/*.svd"

RDEPENDS_${PN}-data-atmel += "${PN}-atmel-license"

# For Freescale
LICENSE_${PN}-data-freescale = "svd-Freescale"
LICENSE_${PN}-freescale-license = "svd-Freescale"

FILES_${PN}-freescale-license = "${INSTALL_PATH}/data/Freescale/Freescale*CMSIS-SVD*License*Agreement.pdf"
FILES_${PN}-data-freescale = "${INSTALL_PATH}/data/Freescale/*.svd"

RDEPENDS_${PN}-data-freescale += "${PN}-freescale-license"

# For Fujitsu
LICENSE_${PN}-data-fujitsu = "svd-Fujitsu"
LICENSE_${PN}-fujitsu-license = "svd-Fujitsu"

FILES_${PN}-fujitsu-license = "${INSTALL_PATH}/data/Fujitsu/License.html"
FILES_${PN}-data-fujitsu = "${INSTALL_PATH}/data/Fujitsu/*.svd"

RDEPENDS_${PN}-data-fujitsu += "${PN}-fujitsu-license"

# For Holtek
LICENSE_${PN}-data-holtek = "Apache-2.0"

FILES_${PN}-data-holtek = "${INSTALL_PATH}/data/Holtek/*.svd"

RDEPENDS_${PN}-data-holtek += "${PN}-apache-license"

# For Nordic
LICENSE_${PN}-data-nordic = "Apache-2.0"

FILES_${PN}-data-nordic = "${INSTALL_PATH}/data/Nordic/*.svd"

RDEPENDS_${PN}-data-nordic += "${PN}-apache-license"

# For Nuvoton
LICENSE_${PN}-data-nuvoton = "Apache-2.0"

FILES_${PN}-data-nuvoton = "${INSTALL_PATH}/data/Nuvoton/*.svd"

RDEPENDS_${PN}-data-nuvoton += "${PN}-apache-license"

# For NXP
LICENSE_${PN}-data-nxp = "Apache-2.0"

FILES_${PN}-data-nxp = "${INSTALL_PATH}/data/NXP/*.svd"

RDEPENDS_${PN}-data-nxp += "${PN}-apache-license"

# For SiliconLabs
LICENSE_${PN}-data-siliconlabs = "Apache-2.0"

FILES_${PN}-data-siliconlabs = "${INSTALL_PATH}/data/SiliconLabs/*.svd"

RDEPENDS_${PN}-data-siliconlabs += "${PN}-apache-license"

# For Spansion
LICENSE_${PN}-data-spansion = "Apache-2.0"

FILES_${PN}-data-spansion = "${INSTALL_PATH}/data/Spansion/*.svd"

RDEPENDS_${PN}-data-spansion += "${PN}-apache-license"

# For STMicro
LICENSE_${PN}-data-stmicro = "svd-STMicro"
LICENSE_${PN}-stmicro-license = "svd-STMicro"

FILES_${PN}-stmicro-license = "${INSTALL_PATH}/data/STMicro/License.html"
FILES_${PN}-data-stmicro = "${INSTALL_PATH}/data/STMicro/*.svd"

RDEPENDS_${PN}-data-stmicro += "${PN}-stmicro-license"

# For TexasInstruments
LICENSE_${PN}-data-texasinstruments = "Apache-2.0"

FILES_${PN}-data-texasinstruments = "${INSTALL_PATH}/data/TexasInstruments/*.svd"

RDEPENDS_${PN}-data-texasinstruments += "${PN}-apache-license"

# For Toshiba
LICENSE_${PN}-data-toshiba = "Apache-2.0"

FILES_${PN}-data-toshiba = "${INSTALL_PATH}/data/Toshiba/*.svd"

RDEPENDS_${PN}-data-toshiba += "${PN}-apache-license"

# Make cmsis-svd package depend on all of the split-out packages
python populate_packages_prepend () {
    firmware_pkgs = oe.utils.packages_filter_out_system(d)
    d.appendVar('RDEPENDS_cmsis-svd', ' ' + ' '.join(firmware_pkgs))
    d.appendVar('RRECOMMENDS_cmsis-svd_append_class-nativesdk', ' ' + ' '.join(firmware_pkgs))
}
# Make sure also to add python dependencies
RRECOMMENDS_${PN}_append_class-nativesdk = " nativesdk-python3-json "
RRECOMMENDS_${PN}_append_class-nativesdk = " nativesdk-python3-setuptools "
RRECOMMENDS_${PN}_append_class-nativesdk = " nativesdk-python3-six "
RRECOMMENDS_${PN}_append_class-nativesdk = " nativesdk-python3-xml "

# Make sure to create the cmsis-svd package
ALLOW_EMPTY_${PN} = "1"
