Compilation of TF-M (Trusted Firmware-M):
1. Pre-requisite
2. Initialize cross-compilation via SDK
3. Prepare TF-M source code
4. Manage TF-M source code with GIT
5. Compile TF-M source code
6. Update software on board
7. Update starter package with TF-M compilation outputs

----------------
1. Pre-requisite
----------------
OpenSTLinux SDK must be installed.

For TF-M build you need to install:
- git:
    Ubuntu: sudo apt-get install git-core gitk
    Fedora: sudo yum install git

If you have never configured you git configuration:
    $ git config --global user.name "your_name"
    $ git config --global user.email "your_email@example.com"

External device tree is extracted. If this is not the case, please follow the
README_HOW_TO.txt in ../external-dt.

---------------------------------------
2. Initialize cross-compilation via SDK
---------------------------------------
Source SDK environment:
    $ source <path to SDK>/environment-setup

To verify that your cross-compilation environment is set-up correctly:
    $ set | grep CROSS_COMPILE

  If the variable CROSS_COMPILE has a value:
   - arm-ostl-linux-gnueabi- for 32 bits architecture (for example STM32MP1)
   - aarch64-ostl-linux- for 64 bits architecture (for example STM32MP2)
  Then everything is set-up correctly

Warning: the environment are valid only on the shell session where you have
sourced the sdk environment.

----------------------
3. Prepare TF-M source
----------------------
If not already done, extract the sources from Developer Package tarball, for example:
    $ tar xf en.SOURCES-stm32mp*-*.tar.xz

In the TF-M source directory (sources/*/##BP##-##PR##),
you have one TF-M source tarball, the patches as diff tarball and one Makefile:
   - ##BP##-##PR##.tar.xz
   - ##BP##-##PR##-diff.gz
   - Makefile.sdk.##MACHINE##

If you would like to have a full git management for the source code move to
to section 4 [Management of TF-M source code with GIT].

Otherwise, you must simply extract the tarball now:

    $> tar xf ##BP##-##PR##.tar.xz
    $> cd ##BP##
    $> test -d .git || git init . && git add . && git commit -m "TF-M source code"
    $> cd external/stm32_lp_fw; test -d .git || git init . && git add . && git commit -m "STM32MP Low Power fiwmware" && git tag ##ARCHIVER_REVISION_MP2_LOW_POWER## && git gc; cd -
    $> git gc

You can now move to section 5 [Compile TF-M source code].

-----------------------------------
4. Manage TF-M source code with GIT
-----------------------------------
If you like to have a better management of change made on TF-M source, you
have 2 solutions to use git:

4.1 Get STMicroelectronics TF-M source from GitHub
--------------------------------------------------
    URL: https://github.com/STMicroelectronics/arm-trusted-firmware-m.git
    Branch: ##ARCHIVER_ST_BRANCH##
    Revision: ##ARCHIVER_ST_REVISION##

    $ git clone https://github.com/STMicroelectronics/arm-trusted-firmware-m.git
    $ git checkout -b WORKING ##ARCHIVER_ST_REVISION##

With this configuration, we recommend to enable external source code download from
TF-M build process through TF_M_EXTERNAL_SOURCES var:
    $ export TF_M_EXTERNAL_SOURCES=0

4.2 Get Git from Arm Software community and apply STMicroelectronics patches
---------------------------------------------------------------
    URL: git://git.trustedfirmware.org/TF-M/trusted-firmware-m.git
    Branch: ##ARCHIVER_COMMUNITY_BRANCH##
    Revision: ##ARCHIVER_COMMUNITY_REVISION##

    $ git clone https://github.com/ARM-software/arm-trusted-firmware-m.git
    $ cd arm-trusted-firmware-m
    $ git checkout -b WORKING ##ARCHIVER_COMMUNITY_REVISION##

Add external TF-M source code:
    $ cd arm-trusted-firmware-m
    $ git clone https://github.com/laurencelundblade/QCBOR.git ##TF_M_PATH_QCBOR##
    $ cd ##TF_M_PATH_QCBOR##
    $ git checkout -b WORKING ##ARCHIVER_REVISION_QCBOR##
    $ cd arm-trusted-firmware-m
    $ git clone https://github.com/mcu-tools/mcuboot.git ##TF_M_PATH_MCUBOOT##
    $ cd ##TF_M_PATH_MCUBOOT##
    $ git checkout -b WORKING ##ARCHIVER_REVISION_MCUBOOT##
    $ cd arm-trusted-firmware-m
    $ git clone https://github.com/ARM-software/CMSIS_6.git ##TF_M_PATH_CMSIS##
    $ cd ##TF_M_PATH_CMSIS##
    $ git checkout -b WORKING ##ARCHIVER_REVISION_CMSIS##
    $ cd arm-trusted-firmware-m
    $ git clone https://github.com/ARM-software/CMSIS_6.git ##TF_M_PATH_STM32MP2_CMSIS_CORE##
    $ cd ##TF_M_PATH_STM32MP2_CMSIS_CORE##
    $ git checkout -b WORKING ##ARCHIVER_REVISION_STM32MP2_CMSIS_CORE##
    $ cd arm-trusted-firmware-m
    $ git clone https://github.com/Mbed-TLS/mbedtls.git ##TF_M_PATH_MBEDCRYPTO##
    $ cd ##TF_M_PATH_MBEDCRYPTO##
    $ git checkout -b WORKING ##ARCHIVER_REVISION_MBEDCRYPTO##
    $ cd arm-trusted-firmware-m
    $ git clone https://github.com/STMicroelectronics/stm32-ddr-phy-binary.git ##TF_M_PATH_DDR_PHY_BIN_SRC##
    $ cd ##TF_M_PATH_DDR_PHY_BIN_SRC##
    $ git checkout -b WORKING ##ARCHIVER_REVISION_DDR_PHY_BIN_SRC##
    $ cd arm-trusted-firmware-m
    $ git clone https://github.com/STMicroelectronics/psa-adac.git ##TF_M_PATH_PSA_ADAC##
    $ cd ##TF_M_PATH_PSA_ADAC##
    $ git checkout -b WORKING ##ARCHIVER_REVISION_PSA_ADAC##
    $ cd arm-trusted-firmware-m
    $ git clone https://github.com/STMicroelectronics/SCP-firmware.git ##TF_M_PATH_SCP_FW##
    $ cd ##TF_M_PATH_SCP_FW##
    $ git checkout -b WORKING ##ARCHIVER_REVISION_SCP_FW##
    $ cd arm-trusted-firmware-m
    $ git clone ##ARCHIVER_URL_STM32MP2_HAL_DRIVER## ##TF_M_PATH_STM32MP2_HAL_DRIVER##
    $ cd ##TF_M_PATH_STM32MP2_HAL_DRIVER##
    $ git checkout -b WORKING ##ARCHIVER_REVISION_STM32MP2_DRIVER##
    $ cd arm-trusted-firmware-m
    $ git clone ##ARCHIVER_URL_STM32MP2_CMSIS_DEVICE## ##TF_M_PATH_STM32MP2_CMSIS_DEVICE##
    $ cd ##TF_M_PATH_STM32MP2_CMSIS_DEVICE##
    $ git checkout -b WORKING ##ARCHIVER_REVISION_STM32MP2_CMSIS_DEVICE##
    $ cd arm-trusted-firmware-m
    $ cd arm-trusted-firmware-m
    $ git clone ##ARCHIVER_URL_MP2_LOW_POWER## ##TF_M_PATH_STM32MP2_LOW_POWER##
    $ cd ##TF_M_PATH_STM32MP2_LOW_POWER##
    $ git checkout -b WORKING ##ARCHIVER_REVISION_MP2_LOW_POWER##

Apply ST patches:
    $ cd arm-trusted-firmware-m
    $ gzip -dk <path to patch>/##BP##-##PR##-diff.gz && git apply <path to patch>/##BP##-##PR##-diff

---------------------------
5. Compile TF-M source code
---------------------------
According to your needs, there are 2 propositions to generate TF-M artifacts:

5.1 Updating Starter Package artifacts
--------------------------------------
If not already done, extract the artifacts from Starter Package tarball, for example:
    $ tar xf en.FLASH-##MACHINE##-*.tar.xz
Move to Starter Package root folder,
    $ cd <your_starter_package_dir_path>
Cleanup Starter Package from original TF-M artifacts first
    $ rm -rf images/##MACHINE##/arm-trusted-firmware-m/*
    $ rm -rf images/##MACHINE##/m33-firmware/*
Configure the DEPLOYDIR path to Starter Package TF-M artifacts folder
    $ export DEPLOYDIR=<your_starter_package_dir_path>/images/##MACHINE##/arm-trusted-firmware-m
The M33FW_artifacts directory path must be specified before launching compilation
    $ export M33FW_DEPLOYDIR_ROOT=<your_starter_package_dir_path>/images/##MACHINE##

You can now move to section 5.3 [Generating TF-M artifacts].

5.2 Creating your own M33FW artifacts (development use case)
-----------------------------------------------------------------
With this configuration you will need to generate one by one the DDR, non-secure and secure firmware artifacts first before being able to generate
the M33FW artifacts. And for that you need to share the same root folder for tf-m and m33tdprojects compilation under Developer Package
The M33FW_artifacts directory path must be specified before launching compilation
    $> export M33FW_DEPLOYDIR_ROOT=<tf-m and m33tdprojects artifacts location>
Make sure to configure then the DEPLOYDIR path accordingly:
    $> export DEPLOYDIR=${M33FW_DEPLOYDIR_ROOT}/arm-trusted-firmware-m

You can now move to section 5.3 [Generating TF-M artifacts].

5.3 Generating TF-M artifacts
-----------------------------
To use the external device tree feature, EXTDT_DIR variable must be set to the root location of external DT
as specified in the README.HOW_TO.txt of external-dt
    $> export EXTDT_DIR=<external DT location>
Before build, TFM_TESTS_DIR variable must be set to the root location of tf-m-tests-stm32mp source
as specified in the README.HOW_TO.txt of tf-m-tests-stm32mp
    $> export TFM_TESTS_DIR=<tf-m-tests-stm32mp location>

The build results for this component are available in DEPLOYDIR (Default: $PWD/../deploy).
If needed, this deploy directory can be specified by adding "DEPLOYDIR=<your_deploy_dir_path>" compilation option to the build command line below.

For example configure a dedicated deploy directory at the same level of tf-m source code:
    $ cd ##BP##
    $ export DEPLOYDIR=$PWD/../deploy/arm-trusted-firmware-m
    $> mkdir -p ${DEPLOYDIR}

The output folder for the build artifacts is defined as BLD_PATH with default: $PWD/../build.
So it means here that it is configure at the same level of tf-m source code: "BLD_PATH=<tf-m-stm32mp location>/build"
In order to share the TF-M build artifacts with other components, the TF_M_BUILD_PATH variable must be set
as specified in the README.HOW_TO.txt of CubeMp2
    $ export TF_M_BUILD_PATH=<tf-m-stm32mp location>/build

To list TF-M source code compilation configurations:
    $ make -f $PWD/../Makefile.sdk.##MACHINE## help

There are different targets for TF-M compilation:

- Generate TF-M binaries
  Using default configuration, you just need to launch the 'tfm' target:
    $> make -f $PWD/../Makefile.sdk.##MACHINE## clean
    $> make -f $PWD/../Makefile.sdk.##MACHINE## tfm
  Example below for a specific config:
    $ make -f $PWD/../Makefile.sdk.##MACHINE## TF_M_CONFIG=m33td_sdcard TF_M_PLATFORM=stm/stm32mp215f_dk M33FW_DEVICETREE=stm32mp215f-dk-cm33tdcid-ostl clean
    $ make -f $PWD/../Makefile.sdk.##MACHINE## TF_M_CONFIG=m33td_sdcard TF_M_PLATFORM=stm/stm32mp215f_dk M33FW_DEVICETREE=stm32mp215f-dk-cm33tdcid-ostl tfm
  The build results for this component are available in <DEPLOYDIR>.

- Generate M33FW binaires
  Make sure to have all m33 firmwares binaries (CubeMp2 and TF-M) available in <M33FW_DEPLOYDIR_ROOT> folder before launching the build
  Using default configuration, you just need to launch the 'm33fw' target:
    $> make -f $PWD/../Makefile.sdk.##MACHINE## m33fw
  Example below for a specific config:
    $ make -f $PWD/../Makefile.sdk.##MACHINE## TF_M_CONFIG=m33td_sdcard TF_M_PLATFORM=stm/stm32mp215f_dk M33FW_DEVICETREE=stm32mp215f-dk-cm33tdcid-ostl m33fw
  The build results for this component are available in <M33FW_DEPLOYDIR_ROOT>/m33-firmware

---------------------------
6. Update software on board
---------------------------
Please use STM32CubeProgrammer to update the boot partitions, find more informations on the wiki website https://wiki.st.com/stm32mpu

---------------------------
7. Generate new Starter Package with TF-M compilation outputs
---------------------------
If not already done, extract the artifacts from Starter Package tarball, for example:
    # tar xf en.FLASH-stm32mp*-*.tar.xz

Move to Starter Package root folder,
    #> cd <your_starter_package_dir_path>
Cleanup Starter Package from original TF-M artifacts first
    #> rm -rf images/stm32mp*/arm-trusted-firmware-m/*
    #> rm -rf images/stm32mp*/m33-firmware/*
Update Starter Package with new FSBLM and M33DDR binaries from <DEPLOYDIR> folder
    #> cp -rvf ${DEPLOYDIR}/* images/stm32mp*/arm-trusted-firmware-m/
        NB: if <DEPLOYDIR> has not been overriden at compilation step, use default path: <TF-M source code folder>/../deploy
Update Starter Package with new m33-firmware artifacts from <M33FW_DEPLOYDIR_ROOT>/m33-firmware folder:
    #> cp -rvf ${M33FW_DEPLOYDIR_ROOT}/m33-firmware/* images/stm32mp*/m33-firmware/

Then the new Starter Package is ready to use for "Image flashing" on board (more information on wiki website https://wiki.st.com/stm32mpu).

----------------------------
8. Example of compilation usage
----------------------------
    $@E> cd ##BP##-##PR##
    $@E> tar xf ##BP##-##PR##.tar.xz
    $@E> cd ##BP##
    $@E> test -d .git || git init . && git add . && git commit -m "TF-M source code" && git gc
    $@E> cd external/stm32_lp_fw; test -d .git || git init . && git add . && git commit -m "STM32MP Low Power fiwmware" && git tag ##ARCHIVER_REVISION_MP2_LOW_POWER## && git gc; cd -


    $ cd ..
    $ cd ..
    $@P> cd ##BP##-##PR##
    $@P> cd ##BP##

    $@S> export M33FW_DEPLOYDIR_ROOT=<your_deploy_dir_path>
    $@S> export TF_M_BUILD_PATH=$PWD/<your_build_subdir_path>

##CASE_stm32mp2-m33td##    "your_board_name" is like stm32mp257f-dk or stm32mp215f-mx-mycustomboard
##CASE_stm32mp2-m33td##    "your_storage_boot_scheme_cortex_m" is like m33td_emmc, m33td_nor, m33td_nor-emmc, m33td_nor-sdcard, m33td_sdcard
##CASE_stm32mp2-m33td##    "your_m33_profile" is like stm/stm32mp257f_ev1, stm/stm32mp215f-dk
##CASE_stm32mp2-m33td##    "your_cube_m33td_project_name" is the name of project used on M33 processor
##CASE_stm32mp2-m33td##    "your_cube_m33td_project_path" is the path for project used on M33 processor
##CASE_stm32mp2-m33td##    For runtime binaries
##CASE_stm32mp2-m33td##    $@C> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/arm-trusted-firmware-m TF_M_CONFIG=<your_storage_boot_scheme_cortex_m> TF_M_PLATFORM=<your_m33_profile> M33FW_DEVICETREE=<tfm_dt_name> EXTDT_DIR=<externaldt_path> EXTDT_DIR_TF_M=<externaldt_tfm_path> EXTDT_DIR_MCU=<externaldt_mcuboot_path> tfm
##CASE_stm32mp2-m33td##    For runtime M33FW binaries
##CASE_stm32mp2-m33td##    $@M> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/arm-trusted-firmware-m TF_M_CONFIG=<your_storage_boot_scheme_cortex_m> M33FW_CONFIG=<your_storage_boot_scheme_cortex_m> M33FW_DEVICETREE=<tfm_dt_name> M33FW_PROJECT_NAME=<your_cube_m33td_project_name> M33FW_PROJECT_PATH=<your_cube_m33td_project_path> m33fw
##CASE_stm32mp2-m33td##    Example with external dt:
##CASE_stm32mp2-m33td##      Example for runtime binaries
##CASE_stm32mp2-m33td##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/arm-trusted-firmware-m TF_M_CONFIG=m33td_sdcard TF_M_PLATFORM=stm/stm32mp215f_dk M33FW_DEVICETREE=stm32mp215f-dk-cm33tdcid-ostl   EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_M=cm33-td/tfm EXTDT_DIR_MCU=cm33-td/mcuboot tfm
##CASE_stm32mp2-m33td##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/arm-trusted-firmware-m TF_M_CONFIG=m33td_sdcard TF_M_PLATFORM=stm/stm32mp257f_ev1 M33FW_DEVICETREE=stm32mp257f-ev1-cm33tdcid-ostl EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_M=cm33-td/tfm EXTDT_DIR_MCU=cm33-td/mcuboot tfm
##CASE_stm32mp2-m33td##      Example for M33FW binaries
##CASE_stm32mp2-m33td##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/arm-trusted-firmware-m TF_M_CONFIG=m33td_sdcard M33FW_CONFIG=m33td_sdcard M33FW_DEVICETREE=stm32mp215f-dk-cm33tdcid-ostl  M33FW_PROJECT_NAME=StarterApp_M33TD M33FW_PROJECT_PATH=Projects/STM32MP215F-DK/Demonstrations/StarterApp_M33TD  m33fw
##CASE_stm32mp2-m33td##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/arm-trusted-firmware-m TF_M_CONFIG=m33td_sdcard M33FW_CONFIG=m33td_sdcard M33FW_DEVICETREE=stm32mp257f-ev1-cm33tdcid-ostl M33FW_PROJECT_NAME=StarterApp_M33TD M33FW_PROJECT_PATH=Projects/STM32MP257F-EV1/Demonstrations/StarterApp_M33TD m33fw
##CASE_stm32mp2-m33td##    Example with CubeMx devicetree:
##CASE_stm32mp2-m33td##      Example for runtime binaries
##CASE_stm32mp2-m33td##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/arm-trusted-firmware-m TF_M_CONFIG=m33td_sdcard TF_M_PLATFORM=stm/stm32mp215f_dk M33FW_DEVICETREE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_M=CM33/tfm EXTDT_DIR_MCU=CM33/DeviceTree/<your_cubemx_project_name>/mcuboot tfm
##CASE_stm32mp2-m33td##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/arm-trusted-firmware-m TF_M_CONFIG=m33td_sdcard TF_M_PLATFORM=stm/stm32mp257f_ev1 M33FW_DEVICETREE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_M=CM33/tfm EXTDT_DIR_MCU=CM33/DeviceTree/<your_cubemx_project_name>/mcuboot tfm
##CASE_stm32mp2-m33td##      Example for M33FW binaries
##CASE_stm32mp2-m33td##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/arm-trusted-firmware-m TF_M_CONFIG=m33td_sdcard M33FW_CONFIG=m33td_sdcard M33FW_DEVICETREE=<your_CUBE_MX_board_name> M33FW_PROJECT_NAME=M33TD_NSAppCore M33FW_PROJECT_PATH=. m33fw
##CASE_stm32mp2-m33td##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/arm-trusted-firmware-m TF_M_CONFIG=m33td_sdcard M33FW_CONFIG=m33td_sdcard M33FW_DEVICETREE=<your_CUBE_MX_board_name> M33FW_PROJECT_NAME=M33TD_NSAppCore M33FW_PROJECT_PATH=. m33fw

