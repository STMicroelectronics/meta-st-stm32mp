# Add stm32mp1 support
SRC_URI:append:stm32mpcommon = " file://STM32MP13xx.svd;subdir=git/data/STMicro"
SRC_URI:append:stm32mpcommon = " file://STM32MP15xxx.svd;subdir=git/data/STMicro"

# Add the same for nativesdk
SRC_URI:append:class-nativesdk = " file://STM32MP13xx.svd;subdir=git/data/STMicro"
SRC_URI:append:class-nativesdk = " file://STM32MP15xxx.svd;subdir=git/data/STMicro"
