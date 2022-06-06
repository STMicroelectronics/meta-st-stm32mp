SUMMARY = "CMSIS SVD data files and parser"

LICENSE = " Apache-2.0 & svd-Atmel & svd-Freescale & svd-Fujitsu & svd-STMicro "

LIC_FILES_CHKSUM = "\
    file://LICENSE-APACHE;md5=fa818a259cbed7ce8bc2a22d35a464fc \
    file://data/STMicro/License.html;md5=9a2821012ac32bea060eccbc76512bdb \
    file://data/Freescale/Freescale%20CMSIS-SVD%20License%20Agreement.pdf;md5=33928757d8c2861dc9256ce344d11db3 \
    file://data/Fujitsu/License.html;md5=e630487a365e7e0c5e03afcc644ce0ad \
    file://data/Atmel/LICENSE;md5=c4400c3a321c71218e903363e6f28890 \
    "

NO_GENERIC_LICENSE[svd-Atmel] = "data/Atmel/LICENSE"
NO_GENERIC_LICENSE[svd-Freescale] = "data/Freescale/Freescale CMSIS-SVD License Agreement.pdf"
NO_GENERIC_LICENSE[svd-Fujitsu] = "data/Fujitsu/License.html"
NO_GENERIC_LICENSE[svd-STMicro] = "data/STMicro/License.html"

inherit pkgconfig autotools-brokensep gettext

SRC_URI = "git://github.com/posborne/cmsis-svd.git;protocol=https;branch=master"
SRCREV = "f487b5ca7c132b8f09d11514c509372f83a6cb75"

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
FILES:${PN}:class-target = ""

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
LICENSE:${PN}-parser = "Apache-2.0"
LICENSE:${PN}-apache-license = "Apache-2.0"

FILES:${PN}-apache-license = "${INSTALL_PATH}/LICENSE-APACHE"
FILES:${PN}-parser = "${INSTALL_PATH}/*.py ${INSTALL_PATH}/examples ${INSTALL_PATH}/tests ${INSTALL_PATH}/data/ARM_SAMPLE/*.svd"

RDEPENDS:${PN}-parser += "${PN}-apache-license"
# For python dependencies
RDEPENDS:${PN}-parser += "python3-json"
RDEPENDS:${PN}-parser += "python3-setuptools"
RDEPENDS:${PN}-parser += "python3-six"
RDEPENDS:${PN}-parser += "python3-xml"

# For Atmel
LICENSE:${PN}-data-atmel = "svd-Atmel"
LICENSE:${PN}-atmel-license = "svd-Atmel"

FILES:${PN}-atmel-license = "${INSTALL_PATH}/data/Atmel/LICENSE"
FILES:${PN}-data-atmel = "${INSTALL_PATH}/data/Atmel/*.svd"

RDEPENDS:${PN}-data-atmel += "${PN}-atmel-license"

# For Freescale
LICENSE:${PN}-data-freescale = "svd-Freescale"
LICENSE:${PN}-freescale-license = "svd-Freescale"

FILES:${PN}-freescale-license = "${INSTALL_PATH}/data/Freescale/Freescale*CMSIS-SVD*License*Agreement.pdf"
FILES:${PN}-data-freescale = "${INSTALL_PATH}/data/Freescale/*.svd"

RDEPENDS:${PN}-data-freescale += "${PN}-freescale-license"

# For Fujitsu
LICENSE:${PN}-data-fujitsu = "svd-Fujitsu"
LICENSE:${PN}-fujitsu-license = "svd-Fujitsu"

FILES:${PN}-fujitsu-license = "${INSTALL_PATH}/data/Fujitsu/License.html"
FILES:${PN}-data-fujitsu = "${INSTALL_PATH}/data/Fujitsu/*.svd"

RDEPENDS:${PN}-data-fujitsu += "${PN}-fujitsu-license"

# For Holtek
LICENSE:${PN}-data-holtek = "Apache-2.0"

FILES:${PN}-data-holtek = "${INSTALL_PATH}/data/Holtek/*.svd"

RDEPENDS:${PN}-data-holtek += "${PN}-apache-license"

# For Nordic
LICENSE:${PN}-data-nordic = "Apache-2.0"

FILES:${PN}-data-nordic = "${INSTALL_PATH}/data/Nordic/*.svd"

RDEPENDS:${PN}-data-nordic += "${PN}-apache-license"

# For Nuvoton
LICENSE:${PN}-data-nuvoton = "Apache-2.0"

FILES:${PN}-data-nuvoton = "${INSTALL_PATH}/data/Nuvoton/*.svd"

RDEPENDS:${PN}-data-nuvoton += "${PN}-apache-license"

# For NXP
LICENSE:${PN}-data-nxp = "Apache-2.0"

FILES:${PN}-data-nxp = "${INSTALL_PATH}/data/NXP/*.svd"

RDEPENDS:${PN}-data-nxp += "${PN}-apache-license"

# For SiliconLabs
LICENSE:${PN}-data-siliconlabs = "Apache-2.0"

FILES:${PN}-data-siliconlabs = "${INSTALL_PATH}/data/SiliconLabs/*.svd"

RDEPENDS:${PN}-data-siliconlabs += "${PN}-apache-license"

# For Spansion
LICENSE:${PN}-data-spansion = "Apache-2.0"

FILES:${PN}-data-spansion = "${INSTALL_PATH}/data/Spansion/*.svd"

RDEPENDS:${PN}-data-spansion += "${PN}-apache-license"

# For STMicro
LICENSE:${PN}-data-stmicro = "svd-STMicro"
LICENSE:${PN}-stmicro-license = "svd-STMicro"

FILES:${PN}-stmicro-license = "${INSTALL_PATH}/data/STMicro/License.html"
FILES:${PN}-data-stmicro = "${INSTALL_PATH}/data/STMicro/*.svd"

RDEPENDS:${PN}-data-stmicro += "${PN}-stmicro-license"

# For TexasInstruments
LICENSE:${PN}-data-texasinstruments = "Apache-2.0"

FILES:${PN}-data-texasinstruments = "${INSTALL_PATH}/data/TexasInstruments/*.svd"

RDEPENDS:${PN}-data-texasinstruments += "${PN}-apache-license"

# For Toshiba
LICENSE:${PN}-data-toshiba = "Apache-2.0"

FILES:${PN}-data-toshiba = "${INSTALL_PATH}/data/Toshiba/*.svd"

RDEPENDS:${PN}-data-toshiba += "${PN}-apache-license"

# Make cmsis-svd package depend on all of the split-out packages
python populate_packages:prepend () {
    firmware_pkgs = oe.utils.packages_filter_out_system(d)
    d.appendVar('RDEPENDS:cmsis-svd', ' ' + ' '.join(firmware_pkgs))
    d.appendVar('RRECOMMENDS:cmsis-svd:append:class-nativesdk', ' ' + ' '.join(firmware_pkgs))
}
# Make sure also to add python dependencies
RRECOMMENDS:${PN}:append:class-nativesdk = " nativesdk-python3-json "
RRECOMMENDS:${PN}:append:class-nativesdk = " nativesdk-python3-setuptools "
RRECOMMENDS:${PN}:append:class-nativesdk = " nativesdk-python3-six "
RRECOMMENDS:${PN}:append:class-nativesdk = " nativesdk-python3-xml "

# Make sure to create the cmsis-svd package
ALLOW_EMPTY:${PN} = "1"
