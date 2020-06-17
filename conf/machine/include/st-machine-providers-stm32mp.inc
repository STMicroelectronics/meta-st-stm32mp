# =========================================================================
# Kernel
# =========================================================================
PREFERRED_PROVIDER_virtual/kernel = "linux-stm32mp"

# =========================================================================
# u-boot
# =========================================================================
PREFERRED_PROVIDER_virtual/bootloader = "u-boot-stm32mp"
PREFERRED_PROVIDER_u-boot = "u-boot-stm32mp"

# =========================================================================
# trusted-firmware-a
# =========================================================================
PREFERRED_PROVIDER_virtual/trusted-firmware-a = "tf-a-stm32mp"
PREFERRED_PROVIDER_virtual/trusted-firmware-a-serialboot = "tf-a-stm32mp-serialboot"
# Define default TF-A config
TF_A_CONFIG_append_pn-tf-a-stm32mp-serialboot = " serialboot "
TF_A_CONFIG_append_pn-tf-a-stm32mp = " ${@bb.utils.contains('BOOTSCHEME_LABELS', 'trusted', 'trusted', '', d)} "
TF_A_CONFIG_append_pn-tf-a-stm32mp = " ${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', 'optee', '', d)} "

# =========================================================================
# optee-os
# =========================================================================
PREFERRED_PROVIDER_virtual/optee-os = "optee-os-stm32mp"

