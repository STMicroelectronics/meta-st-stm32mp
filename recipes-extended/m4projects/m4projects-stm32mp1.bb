SUMMARY = "STM32MP1 Firmware examples for CM4"
LICENSE = "Apache-2.0 & MIT & BSD-3-Clause"
LIC_FILES_CHKSUM = "file://License.md;md5=532c0d9fc2820ec1304ab8e0f227acc7"

SRC_URI = "git://github.com/STMicroelectronics/STM32CubeMP1.git;protocol=https;branch=master"
SRCREV  = "b9a31179d5bf80b3958c3653153bfd4c3a7fc5d5"

PV = "1.6.0"

S = "${WORKDIR}/git"

require recipes-extended/m4projects/m4projects.inc

PROJECTS_LIST_EV1 = " \
	STM32MP157C-EV1/Examples/ADC/ADC_SingleConversion_TriggerTimer_DMA \
	STM32MP157C-EV1/Examples/Cortex/CORTEXM_MPU \
	STM32MP157C-EV1/Examples/CRC/CRC_UserDefinedPolynomial \
	STM32MP157C-EV1/Examples/CRYP/CRYP_AES_DMA \
	STM32MP157C-EV1/Examples/DAC/DAC_SimpleConversion \
	STM32MP157C-EV1/Examples/DMA/DMA_FIFOMode \
	STM32MP157C-EV1/Examples/GPIO/GPIO_EXTI \
	STM32MP157C-EV1/Examples/HASH/HASH_SHA224SHA256_DMA \
	STM32MP157C-EV1/Examples/I2C/I2C_TwoBoards_ComDMA \
	STM32MP157C-EV1/Examples/I2C/I2C_TwoBoards_ComIT \
	STM32MP157C-EV1/Examples/PWR/PWR_STOP_CoPro \
	STM32MP157C-EV1/Examples/QSPI/QSPI_ReadWrite_IT \
	STM32MP157C-EV1/Examples/SPI/SPI_FullDuplex_ComDMA_Master \
	STM32MP157C-EV1/Examples/SPI/SPI_FullDuplex_ComDMA_Slave \
	STM32MP157C-EV1/Examples/TIM/TIM_DMABurst \
	STM32MP157C-EV1/Examples/UART/UART_TwoBoards_ComIT \
	STM32MP157C-EV1/Examples/UART/UART_Receive_Transmit_Console \
	STM32MP157C-EV1/Examples/WWDG/WWDG_Example \
	STM32MP157C-EV1/Applications/OpenAMP/OpenAMP_Dynamic_ResMgr \
	STM32MP157C-EV1/Applications/OpenAMP/OpenAMP_raw \
	STM32MP157C-EV1/Applications/OpenAMP/OpenAMP_TTY_echo \
	STM32MP157C-EV1/Applications/OpenAMP/OpenAMP_TTY_echo_wakeup \
	STM32MP157C-EV1/Applications/FreeRTOS/FreeRTOS_ThreadCreation \
	STM32MP157C-EV1/Applications/CoproSync/CoproSync_ShutDown \
	STM32MP157C-EV1/Demonstrations/AI_Character_Recognition \
"

PROJECTS_LIST_DK2 = " \
	STM32MP157C-DK2/Examples/ADC/ADC_SingleConversion_TriggerTimer_DMA \
	STM32MP157C-DK2/Examples/Cortex/CORTEXM_MPU \
	STM32MP157C-DK2/Examples/CRC/CRC_UserDefinedPolynomial \
	STM32MP157C-DK2/Examples/CRYP/CRYP_AES_DMA \
	STM32MP157C-DK2/Examples/DMA/DMA_FIFOMode \
	STM32MP157C-DK2/Examples/GPIO/GPIO_EXTI \
	STM32MP157C-DK2/Examples/HASH/HASH_SHA224SHA256_DMA \
	STM32MP157C-DK2/Examples/I2C/I2C_TwoBoards_ComIT \
	STM32MP157C-DK2/Examples/LPTIM/LPTIM_PulseCounter \
	STM32MP157C-DK2/Examples/PWR/PWR_STOP_CoPro \
	STM32MP157C-DK2/Examples/SPI/SPI_FullDuplex_ComDMA_Master \
	STM32MP157C-DK2/Examples/SPI/SPI_FullDuplex_ComDMA_Slave \
	STM32MP157C-DK2/Examples/SPI/SPI_FullDuplex_ComIT_Master \
	STM32MP157C-DK2/Examples/SPI/SPI_FullDuplex_ComIT_Slave \
	STM32MP157C-DK2/Examples/TIM/TIM_DMABurst \
	STM32MP157C-DK2/Examples/UART/UART_TwoBoards_ComDMA \
	STM32MP157C-DK2/Examples/UART/UART_TwoBoards_ComIT \
	STM32MP157C-DK2/Examples/UART/UART_Receive_Transmit_Console \
	STM32MP157C-DK2/Examples/WWDG/WWDG_Example \
	STM32MP157C-DK2/Applications/OpenAMP/OpenAMP_raw \
	STM32MP157C-DK2/Applications/OpenAMP/OpenAMP_TTY_echo \
	STM32MP157C-DK2/Applications/FreeRTOS/FreeRTOS_ThreadCreation \
	STM32MP157C-DK2/Applications/CoproSync/CoproSync_ShutDown \
	STM32MP157C-DK2/Applications/OpenAMP/OpenAMP_TTY_echo_wakeup \
	STM32MP157C-DK2/Demonstrations/AI_Character_Recognition \
"

PROJECTS_LIST = "${PROJECTS_LIST_EV1} ${PROJECTS_LIST_DK2}"
