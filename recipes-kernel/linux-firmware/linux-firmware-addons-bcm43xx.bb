# Copyright (C) 2025 STMicroelectronics - All Rights Reserved#
SUMMARY = "Addons firmware for BCM4343"
HOMEPAGE = "https://github.com/murata-wireless"
LICENSE = "Cypress-bcm43xx"
LIC_FILES_CHKSUM = "file://LICENCE.cypress;md5=cbc5f665d04f741f1e006d2096236ba7"

NO_GENERIC_LICENSE[Cypress-bcm43xx] = "LICENCE.cypress"

inherit allarch

SRC_URI = " \
    git://github.com/murata-wireless/cyw-bt-patch;protocol=https;branch=master;name=bt \
    git://github.com/murata-wireless/cyw-fmac-nvram.git;protocol=https;nobranch=1;name=nvram;destsuffix=nvram-murata \
    git://github.com/murata-wireless/cyw-fmac-fw.git;protocol=https;nobranch=1;name=murata;destsuffix=murata \
    "
SRCREV_bt = "bbc63f8b15394023c4a2fd9f74565fbd0d76ae71"
SRCREV_nvram = "61b41349b5aa95227b4d2562e0d0a06ca97a6959"
SRCREV_murata = "a80cb77798a8d57f15b7c3fd2be65553d9bd5125"
SRCREV_FORMAT = "murata"

PV = "6.0"

do_install() {
   install -d ${D}${nonarch_base_libdir}/firmware/brcm/
   # ---- 43430-----
   # Install calibration file
   install -m 0644 ${UNPACKDIR}/nvram-murata/cyfmac43430-sdio.1DX.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.txt
   # disable Wakeup on WLAN
   sed -i "s/muxenab=\(.*\)$/#muxenab=\1/g" ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.txt
   # Install calibration file (stm32mp15)
   install -m 0644 ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.st,stm32mp157c-dk2.txt
   install -m 0644 ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.st,stm32mp157f-dk2.txt
   # Install calibration file (stm32mp13)
   install -m 0644 ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.st,stm32mp135f-dk.txt

   # Take newest murata firmware
   install -m 0644 ${UNPACKDIR}/murata/cyfmac43430-sdio.bin ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.bin
   install -m 0644 ${UNPACKDIR}/murata/cyfmac43430-sdio.1DX.clm_blob ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.clm_blob

   # Add symlinks for newest kernel compatibility
   cd ${D}${nonarch_base_libdir}/firmware/brcm/
   ln -sf brcmfmac43430-sdio.bin brcmfmac43430-sdio.st,stm32mp157c-dk2.bin
   ln -sf brcmfmac43430-sdio.bin brcmfmac43430-sdio.st,stm32mp157f-dk2.bin
   ln -sf brcmfmac43430-sdio.bin brcmfmac43430-sdio.st,stm32mp135f-dk.bin

   # ---- 43439-----
   # Install calibration file
   install -m 0644 ${UNPACKDIR}/nvram-murata/cyfmac43439-sdio.1YN.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.txt
   # disable Wakeup on WLAN
   sed -i "s/muxenab=\(.*\)$/#muxenab=\1/g" ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.txt
   # Install calibration file (stm32mp25)
   install -m 0644 ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.st,stm32mp257f-dk.txt
   install -m 0644 ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.st,stm32mp235f-dk.txt

   # Take newest murata firmware
   install -m 0644 ${UNPACKDIR}/murata/cyfmac43439-sdio.bin ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.bin
   install -m 0644 ${UNPACKDIR}/murata/cyfmac43439-sdio.1YN.clm_blob ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.clm_blob

   # Add symlinks for newest kernel compatibility
   cd ${D}${nonarch_base_libdir}/firmware/brcm/
   ln -sf brcmfmac43439-sdio.bin brcmfmac43439-sdio.st,stm32mp257f-dk.bin
   ln -sf brcmfmac43439-sdio.bin brcmfmac43439-sdio.st,stm32mp257f-dk-ca35tdcid-ostl.bin
   ln -sf brcmfmac43439-sdio.bin brcmfmac43439-sdio.st,stm32mp257f-dk-ca35tdcid-ostl-m33-examples.bin
   ln -sf brcmfmac43439-sdio.bin brcmfmac43439-sdio.st,stm32mp235f-dk.bin

   # ---- 4773 ----
   # Install calibration file
   install -m 0644 ${UNPACKDIR}/nvram-murata/cyfmac4373-sdio.2AE.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.txt
   # disable Wakeup on WLAN
   sed -i "s/muxenab=\(.*\)$/#muxenab=\1/g" ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.txt
   # Install calibration file (stm32mp25)
   install -m 0644 ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.st,stm32mp215f-dk.txt
   install -m 0644 ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.st,stm32mp215f-dk-ca35tdcid-ostl.txt

   # Take newest murata firmware
   install -m 0644 ${UNPACKDIR}/murata/cyfmac4373-sdio.2AE.bin ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.bin
   install -m 0644 ${UNPACKDIR}/murata/cyfmac4373-sdio.2AE.clm_blob ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.clm_blob

   install -m 644 ${S}/BCM4373A0_001.001.025.0103.0155.FCC.CE.2AE.hcd ${D}${nonarch_base_libdir}/firmware/brcm/BCM4373A0.hcd
   cd ${D}${nonarch_base_libdir}/firmware/brcm/
   ln -sf BCM4373A0.hcd BCM.st,stm32mp215f-dk.hcd

   # Add symlinks for newest kernel compatibility
   cd ${D}${nonarch_base_libdir}/firmware/brcm/
   ln -sf brcmfmac4373-sdio.bin brcmfmac4373-sdio.st,stm32mp215f-dk.bin
   ln -sf brcmfmac4373-sdio.bin brcmfmac4373-sdio.st,stm32mp215f-dk-ca35tdcid-ostl.bin
}

do_install:append:stm32mp1common() {
    install -d ${D}${nonarch_base_libdir}/firmware/brcm/

    # 43430
    install -m 644 ${S}/BCM43430A1_001.002.009.0159.0528.1DX.hcd ${D}${nonarch_base_libdir}/firmware/brcm/BCM43430A1.hcd
    install -m 644 ${S}/LICENCE.cypress ${D}${nonarch_base_libdir}/firmware/LICENCE.cypress_bcm4343
    cd ${D}${nonarch_base_libdir}/firmware/brcm/
    ln -sf BCM43430A1.hcd BCM.st,stm32mp157f-dk2.hcd
    ln -sf BCM43430A1.hcd BCM.st,stm32mp135f-dk.hcd
}
do_install:append:stm32mp2common() {
    install -d ${D}${nonarch_base_libdir}/firmware/brcm/
    install -m 644 ${S}/LICENCE.cypress ${D}${nonarch_base_libdir}/firmware/LICENCE.cypress_bcm4343

    # 43439
    install -m 644 ${S}/CYW4343A2_001.003.016.0031.0000.1YN.hcd ${D}${nonarch_base_libdir}/firmware/brcm/BCM4343A2.hcd
    cd ${D}${nonarch_base_libdir}/firmware/brcm/
    ln -sf BCM4343A2.hcd BCM.st,stm32mp257f-dk.hcd
    ln -sf BCM4343A2.hcd BCM.st,stm32mp257f-dk-ca35tdcid-ostl.hcd
    ln -sf BCM4343A2.hcd BCM.st,stm32mp257f-dk-ca35tdcid-ostl-m33-examples.hcd
    ln -sf BCM4343A2.hcd BCM.st,stm32mp235f-dk.hcd

}
do_install:append:stm32mp21common() {
    install -d ${D}${nonarch_base_libdir}/firmware/brcm/

    # 4373
    install -m 644 ${S}/LICENCE.cypress ${D}${nonarch_base_libdir}/firmware/LICENCE.cypress_bcm4373
    install -m 644 ${S}/BCM4373A0_001.001.025.0103.0155.FCC.CE.2AE.hcd ${D}${nonarch_base_libdir}/firmware/brcm/BCM4373A0.hcd
    cd ${D}${nonarch_base_libdir}/firmware/brcm/
    ln -sf BCM4373A0.hcd BCM.st,stm32mp215f-dk.hcd

    # Install calibration file (stm32mp25)
    install -m 0644 ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.st,stm32mp215f-dk.txt
    install -m 0644 ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.st,stm32mp215f-dk-ca35tdcid-ostl.txt

    ln -sf brcmfmac4373-sdio.bin brcmfmac4373-sdio.st,stm32mp215f-dk.bin
    ln -sf brcmfmac4373-sdio.bin brcmfmac4373-sdio.st,stm32mp215f-dk-ca35tdcid-ostl.bin
}

do_install:append:stm32mp2aarch32common() {
    install -d ${D}${nonarch_base_libdir}/firmware/brcm/

    # 4373
    install -m 644 ${S}/LICENCE.cypress ${D}${nonarch_base_libdir}/firmware/LICENCE.cypress_bcm4373
    install -m 644 ${S}/BCM4373A0_001.001.025.0103.0155.FCC.CE.2AE.hcd ${D}${nonarch_base_libdir}/firmware/brcm/BCM4373A0.hcd
    cd ${D}${nonarch_base_libdir}/firmware/brcm/
    ln -sf BCM4373A0.hcd BCM.st,stm32mp215f-dk-aarch32.hcd

    # Install calibration file (stm32mp25)
    install -m 0644 ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.st,stm32mp215f-dk-aarch32.txt

    ln -sf brcmfmac4373-sdio.bin brcmfmac4373-sdio.st,stm32mp215f-dk-aarch32.bin
}
do_install:append:stm32mp2m33tdcommon() {
    install -d ${D}${nonarch_base_libdir}/firmware/brcm/

    # create link for stm32mp2 M33TD
    # Install calibration file (stm32mp25)
    install -m 0644 ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.st,stm32mp235f-dk-cm33tdcid-ostl-sdcard.bin
    # Add symlinks for newest kernel compatibility
    cd ${D}${nonarch_base_libdir}/firmware/brcm/
     ln -sf brcmfmac43439-sdio.bin brcmfmac43439-sdio.st,stm32mp235f-dk-cm33tdcid-ostl-sdcard.bin

    # 4373
    install -m 644 ${S}/LICENCE.cypress ${D}${nonarch_base_libdir}/firmware/LICENCE.cypress_bcm4373
    install -m 644 ${S}/BCM4373A0_001.001.025.0103.0155.FCC.CE.2AE.hcd ${D}${nonarch_base_libdir}/firmware/brcm/BCM4373A0.hcd
    cd ${D}${nonarch_base_libdir}/firmware/brcm/
    ln -sf BCM4373A0.hcd BCM.st,stm32mp215f-dk-cm33tdcid-ostl-sdcard.hcd

    # Install calibration file (stm32mp25)
    install -m 0644 ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.st,stm32mp215f-dk-cm33tdcid-ostl-sdcard.txt

    ln -sf brcmfmac4373-sdio.bin brcmfmac4373-sdio.st,stm32mp215f-dk-cm33tdcid-ostl-sdcard.bin
}

PACKAGES =+ "${PN}-cypress-license"

LICENSE:${PN} = "Cypress-bcm43xx"
LICENSE:${PN}-cypress-license = "Cypress-bcm43xx"

FILES:${PN}-cypress-license = "${nonarch_base_libdir}/firmware/LICENCE.cypress*"
FILES:${PN} = "${nonarch_base_libdir}/firmware/"

RDEPENDS:${PN} += "${PN}-cypress-license"

RRECOMMENDS:${PN}:append:stm32mpcommon = " ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'bluetooth-suspend', '', d)} "

# Firmware files are generally not ran on the CPU, so they can be
# allarch despite being architecture specific
INSANE_SKIP = "arch"
