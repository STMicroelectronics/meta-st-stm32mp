SUMMARY = "STM32MP1 Firmware examples for CM4"
LICENSE = "Proprietary"

PROJECTS_LIST_EV1 = " \
	'STM32MP157C-EV1/Examples/ADC/ADC_SingleConversion_TriggerTimer_DMA' \
	'STM32MP157C-EV1/Examples/Cortex/CORTEXM_MPU' \
	'STM32MP157C-EV1/Examples/CRC/CRC_UserDefinedPolynomial' \
	'STM32MP157C-EV1/Examples/CRYP/CRYP_AES_DMA' \
	'STM32MP157C-EV1/Examples/DAC/DAC_SimpleConversion' \
	'STM32MP157C-EV1/Examples/DMA/DMA_FIFOMode' \
	'STM32MP157C-EV1/Examples/GPIO/GPIO_EXTI' \
	'STM32MP157C-EV1/Examples/HASH/HASH_SHA224SHA256_DMA' \
	'STM32MP157C-EV1/Examples/I2C/I2C_TwoBoards_ComDMA' \
	'STM32MP157C-EV1/Examples/I2C/I2C_TwoBoards_ComIT' \
	'STM32MP157C-EV1/Examples/QSPI/QSPI_ReadWrite_IT' \
	'STM32MP157C-EV1/Examples/SPI/SPI_FullDuplex_ComDMA_Master' \
	'STM32MP157C-EV1/Examples/SPI/SPI_FullDuplex_ComDMA_Slave' \
	'STM32MP157C-EV1/Examples/TIM/TIM_DMABurst' \
	'STM32MP157C-EV1/Examples/UART/UART_TwoBoards_ComIT' \
	'STM32MP157C-EV1/Examples/WWDG/WWDG_Example' \
	'STM32MP157C-EV1/Applications/OpenAMP/OpenAMP_raw' \
	'STM32MP157C-EV1/Applications/OpenAMP/OpenAMP_TTY_echo' \
	'STM32MP157C-EV1/Applications/OpenAMP/OpenAMP_TTY_echo_wakeup' \
	'STM32MP157C-EV1/Applications/FreeRTOS/FreeRTOS_ThreadCreation' \
	'STM32MP157C-EV1/Applications/CoproSync/CoproSync_ShutDown' \
	'STM32MP157C-EV1/Demonstrations/AI_Character_Recognition' \
"
PROJECTS_LIST_DK2 = " \
	'STM32MP157C-DK2/Examples/ADC/ADC_SingleConversion_TriggerTimer_DMA' \
	'STM32MP157C-DK2/Examples/Cortex/CORTEXM_MPU' \
	'STM32MP157C-DK2/Examples/CRC/CRC_UserDefinedPolynomial' \
	'STM32MP157C-DK2/Examples/CRYP/CRYP_AES_DMA' \
	'STM32MP157C-DK2/Examples/DMA/DMA_FIFOMode' \
	'STM32MP157C-DK2/Examples/GPIO/GPIO_EXTI' \
	'STM32MP157C-DK2/Examples/HASH/HASH_SHA224SHA256_DMA' \
	'STM32MP157C-DK2/Examples/I2C/I2C_TwoBoards_ComIT' \
	'STM32MP157C-DK2/Examples/LPTIM/LPTIM_PulseCounter' \
	'STM32MP157C-DK2/Examples/SPI/SPI_FullDuplex_ComDMA_Master' \
	'STM32MP157C-DK2/Examples/SPI/SPI_FullDuplex_ComDMA_Slave' \
	'STM32MP157C-DK2/Examples/SPI/SPI_FullDuplex_ComIT_Master' \
	'STM32MP157C-DK2/Examples/SPI/SPI_FullDuplex_ComIT_Slave' \
	'STM32MP157C-DK2/Examples/TIM/TIM_DMABurst' \
	'STM32MP157C-DK2/Examples/UART/UART_TwoBoards_ComDMA' \
	'STM32MP157C-DK2/Examples/UART/UART_TwoBoards_ComIT' \
	'STM32MP157C-DK2/Examples/WWDG/WWDG_Example' \
	'STM32MP157C-DK2/Applications/OpenAMP/OpenAMP_raw' \
	'STM32MP157C-DK2/Applications/OpenAMP/OpenAMP_TTY_echo' \
	'STM32MP157C-DK2/Applications/OpenAMP/OpenAMP_TTY_echo_wakeup' \
	'STM32MP157C-DK2/Applications/FreeRTOS/FreeRTOS_ThreadCreation' \
	'STM32MP157C-DK2/Demonstrations/AI_Character_Recognition' \
"

PROJECTS_LIST = "${PROJECTS_LIST_EV1} ${PROJECTS_LIST_DK2}"

require m4projects.inc
