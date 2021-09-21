# Add stm32mp1 support
SRC_URI:append:stm32mpcommon = " file://0001-data-STMicro-add-support-of-stm32mp15xxx.patch"
# Add the same for nativesdk
SRC_URI:append:class-nativesdk = " file://0001-data-STMicro-add-support-of-stm32mp15xxx.patch"
