# Add stm32mp1 support
SRC_URI_append_stm32mpcommon = " file://0001-data-STMicro-add-support-of-stm32mp15xxx.patch"
# Add the same for nativesdk
SRC_URI_append_class-nativesdk = " file://0001-data-STMicro-add-support-of-stm32mp15xxx.patch"
