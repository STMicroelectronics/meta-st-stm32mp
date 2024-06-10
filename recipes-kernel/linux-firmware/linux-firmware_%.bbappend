FILESEXTRAPATHS:prepend:stm32mpcommon := "${THISDIR}/${PN}:"

# Add calibration file
SRC_URI:append:stm32mpcommon = " git://github.com/murata-wireless/cyw-fmac-nvram.git;protocol=https;nobranch=1;name=nvram;destsuffix=nvram-murata "
SRCREV_nvram = "9b7d93eb3e13b2d2ed8ce3a01338ceb54151b77a"
SRC_URI:append:stm32mpcommon = " git://github.com/murata-wireless/cyw-fmac-fw.git;protocol=https;nobranch=1;name=murata;destsuffix=murata "
SRCREV_murata = "e024d0c0a3ab241f547cb44303de7e1b49f0ca78"
SRCREV_FORMAT = "linux-firmware-murata"

do_install:append:stm32mpcommon() {
   # ---- 43430-----
   # Install calibration file
   install -m 0644 ${WORKDIR}/nvram-murata/cyfmac43430-sdio.1DX.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.txt
   # disable Wakeup on WLAN
   sed -i "s/muxenab=\(.*\)$/#muxenab=\1/g" ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.txt
   # Install calibration file (stm32mp15)
   install -m 0644 ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.st,stm32mp157c-dk2.txt
   install -m 0644 ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.st,stm32mp157f-dk2.txt
   # Install calibration file (stm32mp13)
   install -m 0644 ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.st,stm32mp135f-dk.txt

   # Take newest murata firmware
   install -m 0644 ${WORKDIR}/murata/cyfmac43430-sdio.bin ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.bin
   install -m 0644 ${WORKDIR}/murata/cyfmac43430-sdio.1DX.clm_blob ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.clm_blob

   # Add symlinks for newest kernel compatibility
   cd ${D}${nonarch_base_libdir}/firmware/brcm/
   ln -sf brcmfmac43430-sdio.bin brcmfmac43430-sdio.st,stm32mp157c-dk2.bin
   ln -sf brcmfmac43430-sdio.bin brcmfmac43430-sdio.st,stm32mp157f-dk2.bin
   ln -sf brcmfmac43430-sdio.bin brcmfmac43430-sdio.st,stm32mp135f-dk.bin

   # ---- 43439-----
   # Install calibration file
   install -m 0644 ${WORKDIR}/nvram-murata/cyfmac43439-sdio.1YN.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.txt
   # disable Wakeup on WLAN
   sed -i "s/muxenab=\(.*\)$/#muxenab=\1/g" ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.txt
   # Install calibration file (stm32mp25)
   install -m 0644 ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.st,stm32mp257f-dk.txt

   # Take newest murata firmware
   install -m 0644 ${WORKDIR}/murata/cyfmac43439-sdio.bin ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.bin
   install -m 0644 ${WORKDIR}/murata/cyfmac43439-sdio.1YN.clm_blob ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.clm_blob

   # Add symlinks for newest kernel compatibility
   cd ${D}${nonarch_base_libdir}/firmware/brcm/
   ln -sf brcmfmac43439-sdio.bin brcmfmac43439-sdio.st,stm32mp257f-dk.bin
   ln -sf brcmfmac43439-sdio.bin brcmfmac43439-sdio.st,stm32mp257f-dk-ca35tdcid-ostl.bin
   ln -sf brcmfmac43439-sdio.bin brcmfmac43439-sdio.st,stm32mp257f-dk-ca35tdcid-ostl-m33-examples.bin
}

FILES:${PN}-bcm43430:append:stm32mpcommon = " \
  ${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.txt \
  ${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.st,stm32mp157c-dk2.* \
  ${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.st,stm32mp157f-dk2.* \
  ${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.st,stm32mp135f-dk.* \
  ${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.clm_blob \
  ${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.bin \
"

RDEPENDS:${PN}-bcm43430:remove:stm32mpcommon = " ${PN}-cypress-license "

#
PACKAGES =+ "${PN}-bcm43439"
FILES:${PN}-bcm43439:stm32mpcommon = " \
    ${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.txt \
    ${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.bin \
    ${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.clm_blob \
    ${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.st,stm32mp257f-dk* \
 "
