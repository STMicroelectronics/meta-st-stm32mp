SUMMARY = "CMSIS SVD data files"

LICENSE = "Apache-2.0 & \
    svd-AlifSemi & svd-Atmel & svd-Espressif-Community & svd-Freescale & svd-Fujitsu & \
    svd-Kendryte-Community & svd-RaspberryPi & svd-SiFive-Community & svd-STMicro"

LIC_FILES_CHKSUM = "\
    file://LICENSE-APACHE;md5=fa818a259cbed7ce8bc2a22d35a464fc \
    file://data/AlifSemi/License.txt;md5=c7e4ca9de94108a3c05d308c84bdfabf \
    file://data/Atmel/LICENSE;md5=c4400c3a321c71218e903363e6f28890 \
    file://data/Espressif-Community/LICENSE;md5=326c6e1944b7da1aae15812bd62254f5 \
    file://data/Freescale/Freescale%20CMSIS-SVD%20License%20Agreement.pdf;md5=33928757d8c2861dc9256ce344d11db3 \
    file://data/Fujitsu/License.html;md5=e630487a365e7e0c5e03afcc644ce0ad \
    file://data/Kendryte-Community/LICENSE;md5=6d3a2698f0d75d65e3894c6ad904daf5 \
    file://data/RaspberryPi/LICENSE.TXT;md5=db74c933ca4b8bb864b9088bec194057 \
    file://data/SiFive-Community/LICENSE;md5=df905f3e8bdbf47a899d1510502b1212 \
    file://data/STMicro/License.html;md5=9a2821012ac32bea060eccbc76512bdb \
    "

NO_GENERIC_LICENSE[svd-AlifSemi] = "data/AlifSemi/License.txt"
NO_GENERIC_LICENSE[svd-Atmel] = "data/Atmel/LICENSE"
NO_GENERIC_LICENSE[svd-Espressif-Community] = "data/Espressif-Community/LICENSE"
NO_GENERIC_LICENSE[svd-Freescale] = "data/Freescale/Freescale CMSIS-SVD License Agreement.pdf"
NO_GENERIC_LICENSE[svd-Fujitsu] = "data/Fujitsu/License.html"
NO_GENERIC_LICENSE[svd-Kendryte-Community] = "data/Kendryte-Community/LICENSE"
NO_GENERIC_LICENSE[svd-RaspberryPi] = "data/RaspberryPi/LICENSE.TXT"
NO_GENERIC_LICENSE[svd-SiFive-Community] = "data/SiFive-Community/LICENSE"
NO_GENERIC_LICENSE[svd-STMicro] = "data/STMicro/License.html"

SRC_URI = "git://github.com/cmsis-svd/cmsis-svd-data.git;protocol=https;branch=main"
SRCREV = "c65f8551e57c770344d229dcaa0bf838fa29aff4"

PV = "1.0.0+git${@d.getVar('SRCREV')[0:8]}"

BBCLASSEXTEND += "native nativesdk"

PACKAGES += "\
    ${PN}-apache-license \
    \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'AlifSemi', '${PN}-alifsemi ${PN}-alifsemi-license', '', d)} \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'Allwinner-Community', '${PN}-allwinner-community', '', d)} \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'ArteryTek', '${PN}-arterytek', '', d)} \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'Atmel', '${PN}-atmel ${PN}-atmel-license', '', d)} \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'Cypress', '${PN}-cypress', '', d)} \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'Espressif', '${PN}-espressif', '', d)} \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'Espressif-Community', '${PN}-espressif-community ${PN}-espressif-community-license', '', d)} \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'Freescale', '${PN}-freescale ${PN}-freescale-license', '', d)} \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'Fujitsu', '${PN}-fujitsu ${PN}-fujitsu-license', '', d)} \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'GigaDevice', '${PN}-gigadevice', '', d)} \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'Holtek', '${PN}-holtek', '', d)} \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'Infineon', '${PN}-infineon', '', d)} \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'Kendryte-Community', '${PN}-kendryte-community ${PN}-kendryte-community-license', '', d)} \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'Nordic', '${PN}-nordic', '', d)} \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'Nuvoton', '${PN}-nuvoton', '', d)} \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'NXP', '${PN}-nxp', '', d)} \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'RaspberryPi', '${PN}-raspberrypi ${PN}-raspberrypi-license', '', d)} \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'Renesas', '${PN}-renesas', '', d)} \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'SiFive-Community', '${PN}-sifive-community ${PN}-sifive-community-license', '', d)} \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'SiliconLabs', '${PN}-siliconlabs', '', d)} \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'Spansion', '${PN}-spansion', '', d)} \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'STMicro', '${PN}-stmicro ${PN}-stmicro-license', '', d)} \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'TexasInstruments', '${PN}-texasinstruments', '', d)} \
    ${@bb.utils.contains('CMSIS_SVD_DATA', 'Toshiba', '${PN}-toshiba', '', d)} \
    "

CMSIS_SVD_DATA ??= "STMicro"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

INSTALL_PATH = "${datadir}/cmsis-svd/cmsis_svd"

do_install () {
    install -d ${D}${INSTALL_PATH}
    install -m 0644 ${S}/LICENSE-APACHE ${D}${INSTALL_PATH}

    # Filter requested data files
    install -d ${D}${INSTALL_PATH}/data
    for data_path in ${CMSIS_SVD_DATA}
    do
        if [ -d "${S}/data/${data_path}" ]; then
            cp -R ${S}/data/${data_path} ${D}${INSTALL_PATH}/data
        else
            bbwarn "Can not find ${data_path} in ${S}/data"
        fi
    done

    # Remove unexpected files
    find ${D}${INSTALL_PATH}/data -type f \
        \( -name Contents.txt -o -name README.md -o -name "*.py" \) \
        -exec rm -f {} \;
}

# For ARM_SAMPLE svd file example
LICENSE:${PN}-apache-license = "Apache-2.0"
FILES:${PN}-apache-license = "${INSTALL_PATH}/LICENSE-APACHE"

# For AlifSemi
LICENSE:${PN}-alifsemi = "svd-AlifSemi"
FILES:${PN}-alifsemi = "${INSTALL_PATH}/data/AlifSemi/*.svd"
LICENSE:${PN}-alifsemi-license = "svd-AlifSemi"
FILES:${PN}-alifsemi-license = "${INSTALL_PATH}/data/AlifSemi/License.txt"
RDEPENDS:${PN}-alifsemi += "${PN}-alifsemi-license"

# For Allwinner-Community
LICENSE:${PN}-allwinner-community = "Apache-2.0"
FILES:${PN}-allwinner-community = "${INSTALL_PATH}/data/Allwinner-Community/*.svd"

# For ArteryTek
LICENSE:${PN}-arterytek = "Apache-2.0"
FILES:${PN}-arterytek = "${INSTALL_PATH}/data/ArteryTek/*.svd"

# For Atmel
LICENSE:${PN}-atmel = "svd-Atmel"
FILES:${PN}-atmel = "${INSTALL_PATH}/data/Atmel/*.svd"
LICENSE:${PN}-atmel-license = "svd-Atmel"
FILES:${PN}-atmel-license = "${INSTALL_PATH}/data/Atmel/LICENSE"
RDEPENDS:${PN}-atmel += "${PN}-atmel-license"

# For Cypress
LICENSE:${PN}-cypress = "Apache-2.0"
FILES:${PN}-cypress = "${INSTALL_PATH}/data/Cypress/*.svd"

# For Espressif
LICENSE:${PN}-espressif = "Apache-2.0"
FILES:${PN}-espressif = "${INSTALL_PATH}/data/Espressif/*.svd"

# For Espressif Community
LICENSE:${PN}-espressif-community = "svd-Espressif-Community"
FILES:${PN}-espressif-community = "${INSTALL_PATH}/data/Espressif-Community/*.svd"
LICENSE:${PN}-espressif-community-license = "svd-Espressif-Community"
FILES:${PN}-espressif-community-license = "${INSTALL_PATH}/data/Espressif-Community/LICENSE"
RDEPENDS:${PN}-espressif-community += "${PN}-espressif-community-license"

# For Freescale
LICENSE:${PN}-freescale = "svd-Freescale"
FILES:${PN}-freescale = "${INSTALL_PATH}/data/Freescale/*.svd"
LICENSE:${PN}-freescale-license = "svd-Freescale"
FILES:${PN}-freescale-license = "${INSTALL_PATH}/data/Freescale/Freescale*CMSIS-SVD*License*Agreement.pdf"
RDEPENDS:${PN}-freescale += "${PN}-freescale-license"

# For Fujitsu
LICENSE:${PN}-fujitsu = "svd-Fujitsu"
FILES:${PN}-fujitsu = "${INSTALL_PATH}/data/Fujitsu/*.svd"
LICENSE:${PN}-fujitsu-license = "svd-Fujitsu"
FILES:${PN}-fujitsu-license = "${INSTALL_PATH}/data/Fujitsu/License.html"
RDEPENDS:${PN}-fujitsu += "${PN}-fujitsu-license"

# For GigaDevice
LICENSE:${PN}-gigadevice = "Apache-2.0"
FILES:${PN}-gigadevice = "${INSTALL_PATH}/data/GigaDevice/*.svd"

# For Holtek
LICENSE:${PN}-holtek = "Apache-2.0"
FILES:${PN}-holtek = "${INSTALL_PATH}/data/Holtek/*.svd"

# For Infineon
LICENSE:${PN}-infineon = "Apache-2.0"
FILES:${PN}-infineon = "${INSTALL_PATH}/data/Infineon/*.svd"

# For Kendryte-Community
LICENSE:${PN}-kendryte-community = "svd-Kendryte-Community"
FILES:${PN}-kendryte-community = "${INSTALL_PATH}/data/Kendryte-Community/*.svd"
LICENSE:${PN}-kendryte-community-license = "svd-Kendryte-Community"
FILES:${PN}-kendryte-community-license = "${INSTALL_PATH}/data/Kendryte-Community/LICENSE"
RDEPENDS:${PN}-kendryte-community += "${PN}-kendryte-community-license"

# For Nordic
LICENSE:${PN}-nordic = "Apache-2.0"
FILES:${PN}-nordic = "${INSTALL_PATH}/data/Nordic/*.svd"

# For Nuvoton
LICENSE:${PN}-nuvoton = "Apache-2.0"
FILES:${PN}-nuvoton = "${INSTALL_PATH}/data/Nuvoton/*.svd"

# For NXP
LICENSE:${PN}-nxp = "Apache-2.0"
FILES:${PN}-nxp = "${INSTALL_PATH}/data/NXP/*.svd"

# For Raspberry Pi
LICENSE:${PN}-raspberrypi = "svd-RaspberryPi"
FILES:${PN}-raspberrypi = "${INSTALL_PATH}/data/RaspberryPi/*.svd"
LICENSE:${PN}-raspberrypi-license = "svd-RaspberryPi"
FILES:${PN}-raspberrypi-license = "${INSTALL_PATH}/data/RaspberryPi/LICENSE.TXT"
RDEPENDS:${PN}-raspberrypi += "${PN}-raspberrypi-license"

# For Renesas
LICENSE:${PN}-renesas = "Apache-2.0"
FILES:${PN}-renesas = "${INSTALL_PATH}/data/Renesas/*.svd"

# For SiFive Community
LICENSE:${PN}-sifive-community = "svd-SiFive-Community"
FILES:${PN}-sifive-community = "${INSTALL_PATH}/data/SiFive-Community/*.svd"
LICENSE:${PN}-sifive-community-license = "svd-SiFive-Community"
FILES:${PN}-sifive-community-license = "${INSTALL_PATH}/data/SiFive-Community/LICENSE"
RDEPENDS:${PN}-sifive-community += "${PN}-sifive-community-license"

# For Silicon Labs
LICENSE:${PN}-siliconlabs = "Apache-2.0"
FILES:${PN}-siliconlabs = "${INSTALL_PATH}/data/SiliconLabs/Series*/*/*.svd ${INSTALL_PATH}/data/SiliconLabs/SiM3_NRND/*.svd"

# For Spansion
LICENSE:${PN}-spansion = "Apache-2.0"
FILES:${PN}-spansion = "${INSTALL_PATH}/data/Spansion/*.svd"

# For STMicro
LICENSE:${PN}-stmicro = "svd-STMicro"
FILES:${PN}-stmicro = "${INSTALL_PATH}/data/STMicro/*.svd"
LICENSE:${PN}-stmicro-license = "svd-STMicro"
FILES:${PN}-stmicro-license = "${INSTALL_PATH}/data/STMicro/License.html"
RDEPENDS:${PN}-stmicro += "${PN}-stmicro-license"

# For Texas Instruments
LICENSE:${PN}-texasinstruments = "Apache-2.0"
FILES:${PN}-texasinstruments = "${INSTALL_PATH}/data/TexasInstruments/*.svd"

# For Toshiba
LICENSE:${PN}-toshiba = "Apache-2.0"
FILES:${PN}-toshiba = "${INSTALL_PATH}/data/Toshiba/*.svd"

# Make cmsis-svd-data package depend on all of the split-out packages
python cmsis_svd_data_split() {
    firmware_pkgs = oe.utils.packages_filter_out_system(d)
    d.appendVar('RDEPENDS:%s' % d.getVar('PN'), ' ' + ' '.join(firmware_pkgs))
}

PACKAGESPLITFUNCS =+ "cmsis_svd_data_split"

# Make sure to create the cmsis-svd-data package
ALLOW_EMPTY:${PN} = "1"
