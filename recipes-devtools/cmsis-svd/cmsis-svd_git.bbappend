# Add stm32mp1 support
SRC_URI:append:stm32mpcommon = " file://STM32MP13xx.svd;subdir=git/data/STMicro"
SRC_URI:append:stm32mpcommon = " file://STM32MP151x.svd;subdir=git/data/STMicro"
SRC_URI:append:stm32mpcommon = " file://STM32MP153x.svd;subdir=git/data/STMicro"
SRC_URI:append:stm32mpcommon = " file://STM32MP157x.svd;subdir=git/data/STMicro"
# Add stm32mp2 support
SRC_URI:append:stm32mp2common = " file://STM32MP25_CA35.svd;subdir=git/data/STMicro"
SRC_URI:append:stm32mp2common = " file://STM32MP25_CM33.svd;subdir=git/data/STMicro"
SRC_URI:append:stm32mp2common = " file://STM32MP25_CM0P.svd;subdir=git/data/STMicro"

# Add the same for nativesdk
SRC_URI:append:class-nativesdk = " file://STM32MP13xx.svd;subdir=git/data/STMicro"
SRC_URI:append:class-nativesdk = " file://STM32MP151x.svd;subdir=git/data/STMicro"
SRC_URI:append:class-nativesdk = " file://STM32MP153x.svd;subdir=git/data/STMicro"
SRC_URI:append:class-nativesdk = " file://STM32MP157x.svd;subdir=git/data/STMicro"
SRC_URI:append:class-nativesdk = " file://STM32MP25_CA35.svd;subdir=git/data/STMicro"
SRC_URI:append:class-nativesdk = " file://STM32MP25_CM33.svd;subdir=git/data/STMicro"
SRC_URI:append:class-nativesdk = " file://STM32MP25_CM0P.svd;subdir=git/data/STMicro"

