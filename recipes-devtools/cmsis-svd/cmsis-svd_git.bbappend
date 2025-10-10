PACKAGE_ARCH:stm32mpcommon = "${MACHINE_ARCH}"

# Add stm32mp1 support
SRC_URI:append:stm32mpcommon = " file://STM32MP13xx.svd;subdir=${BP}/data/STMicro"
SRC_URI:append:stm32mpcommon = " file://STM32MP151x.svd;subdir=${BP}/data/STMicro"
SRC_URI:append:stm32mpcommon = " file://STM32MP153x.svd;subdir=${BP}/data/STMicro"
SRC_URI:append:stm32mpcommon = " file://STM32MP157x.svd;subdir=${BP}/data/STMicro"
# Add stm32mp2 support
SRC_URI:append:stm32mp2common = " file://STM32MP25_CA35.svd;subdir=${BP}/data/STMicro"
SRC_URI:append:stm32mp2common = " file://STM32MP25_CM33.svd;subdir=${BP}/data/STMicro"
SRC_URI:append:stm32mp2common = " file://STM32MP25_CM0P.svd;subdir=${BP}/data/STMicro"
SRC_URI:append:stm32mp2common = " file://STM32MP21_CA35.svd;subdir=${BP}/data/STMicro"
SRC_URI:append:stm32mp2common = " file://STM32MP21_CM33.svd;subdir=${BP}/data/STMicro"
SRC_URI:append:stm32mp2common = " file://STM32MP23_CA35.svd;subdir=${BP}/data/STMicro"
SRC_URI:append:stm32mp2common = " file://STM32MP23_CM33.svd;subdir=${BP}/data/STMicro"

# Add the same for nativesdk
SRC_URI:append:class-nativesdk = " file://STM32MP13xx.svd;subdir=${BP}/data/STMicro"
SRC_URI:append:class-nativesdk = " file://STM32MP151x.svd;subdir=${BP}/data/STMicro"
SRC_URI:append:class-nativesdk = " file://STM32MP153x.svd;subdir=${BP}/data/STMicro"
SRC_URI:append:class-nativesdk = " file://STM32MP157x.svd;subdir=${BP}/data/STMicro"
SRC_URI:append:class-nativesdk = " file://STM32MP25_CA35.svd;subdir=${BP}/data/STMicro"
SRC_URI:append:class-nativesdk = " file://STM32MP25_CM33.svd;subdir=${BP}/data/STMicro"
SRC_URI:append:class-nativesdk = " file://STM32MP25_CM0P.svd;subdir=${BP}/data/STMicro"
SRC_URI:append:class-nativesdk = " file://STM32MP21_CA35.svd;subdir=${BP}/data/STMicro"
SRC_URI:append:class-nativesdk = " file://STM32MP21_CM33.svd;subdir=${BP}/data/STMicro"
