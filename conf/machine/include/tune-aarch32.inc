DEFAULTTUNE ?= "aarch32"

require conf/machine/include/arm/armv8a/tune-cortexa35.inc

TUNE_PKGARCH_32 = "${ARMPKGARCH}${ARMPKGSFX_THUMB}${ARMPKGSFX_EABI}${ARMPKGSFX_FPU}"

TUNEVALID[aarch32] = "Enable instructions for ARMv8-a"
TUNECONFLICTS[aarch32] = "armv4 armv5 armv6 armv7 armv7a"

MACHINEOVERRIDES =. "${@bb.utils.contains('TUNE_FEATURES', 'aarch32', 'aarch32:', '', d)}"

TUNE_CCARGS .= "${@bb.utils.contains('TUNE_FEATURES', 'aarch32', ' -mcpu=cortex-a35 ', '', d)}"


TUNEVALID[fp-armv8] = "Enable ARMv8 Vector Floating Point unit."
TUNE_CCARGS_MFPU .= "${@bb.utils.contains('TUNE_FEATURES', 'fp-armv8', ' fp-armv8', '', d)}"
TUNE_CCARGS_MFPU .= "${@bb.utils.contains('TUNE_FEATURES', [ 'fp-armv8', 'neon' ], ' neon-fp-armv8', '', d)}"

# Little Endian base configs
AVAILTUNES += "aarch32 aarch32t"
ARMPKGARCH:tune-aarch32                    ?= "aarch32"
ARMPKGARCH:tune-aarch32t                   ?= "aarch32"
TUNE_FEATURES:tune-aarch32                  = "arm aarch32 neon fp-armv8 callconvention-hard"
PACKAGE_EXTRA_ARCHS:tune-aarch32            = "aarch32hf-neon-fp-armv8"

BASE_LIB:tune-aarch32                       = "lib"
BASE_LIB:tune-aarch32t                      = "lib"
