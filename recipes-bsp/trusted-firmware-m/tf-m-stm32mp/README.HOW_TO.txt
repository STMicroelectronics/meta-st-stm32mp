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
TF-M build process through TFM_EXTERNAL_SOURCES var:
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
    $ git clone https://github.com/Mbed-TLS/mbedtls.git ##TF_M_PATH_MBEDCRYPTO##
    $ cd ##TF_M_PATH_MBEDCRYPTO##
    $ git checkout -b WORKING ##ARCHIVER_REVISION_MBEDCRYPTO##
    $ cd arm-trusted-firmware-m
    $ git clone https://github.com/mcu-tools/mcuboot.git ##TF_M_PATH_MCUBOOT##
    $ cd ##TF_M_PATH_MCUBOOT##
    $ git checkout -b WORKING ##ARCHIVER_REVISION_MCUBOOT##
    $ cd arm-trusted-firmware-m
    $ git clone https://github.com/laurencelundblade/QCBOR.git ##TF_M_PATH_QCBOR##
    $ cd ##TF_M_PATH_QCBOR##
    $ git checkout -b WORKING ##ARCHIVER_REVISION_QCBOR##
    $ cd arm-trusted-firmware-m
    $ git clone https://github.com/ARM-software/CMSIS_6.git ##TF_M_PATH_CMSIS##
    $ cd ##TF_M_PATH_CMSIS##
    $ git checkout -b WORKING ##ARCHIVER_REVISION_CMSIS##
    $ cd arm-trusted-firmware-m
    $ git clone https://github.com/STMicroelectronics/stm32-ddr-phy-binary.git ##TF_M_PATH_DDR_PHY_BIN_SRC##
    $ cd ##TF_M_PATH_DDR_PHY_BIN_SRC##
    $ git checkout -b WORKING ##ARCHIVER_REVISION_DDR_PHY_BIN_SRC##
    $ cd arm-trusted-firmware-m
    $ git clone https://github.com/STMicroelectronics/SCP-firmware.git ##TF_M_PATH_SCP_FW##
    $ cd ##TF_M_PATH_SCP_FW##
    $ git checkout -b WORKING ##ARCHIVER_REVISION_SCP_FW##

Apply ST patches:
    $ cd arm-trusted-firmware-m
    $ gzip -dk <path to patch>/##BP##-##PR##-diff.gz && git apply <path to patch>/##BP##-##PR##-diff

---------------------------
5. Compile TF-M source code
---------------------------
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
    $> export DEPLOYDIR=$PWD/../deploy/arm-trusted-firmware-m
    $> export TF_M_BUILD_PATH=$PWD/build
    $> mkdir -p ${DEPLOYDIR}

To list TF-M source code compilation configurations:
    $ make -f $PWD/../Makefile.sdk.##MACHINE## help
To compile TF-M source code:
    $> make -f $PWD/../Makefile.sdk.##MACHINE## all
To compile TF-M source code for a specific config:
    # with TF_M_CONFIG=m33td_sdcard for stm32mp215f-dk
    $ make -f $PWD/../Makefile.sdk.##MACHINE## TF_M_CONFIG=m33td_sdcard TF_M_PLATFORM=stm/stm32mp215f_dk TF_M_DEVICETREE=stm32mp215f-dk-cm33tdcid-ostl  all
    # with TF_M_CONFIG=m33td_nor-sdcard for stm32mp257f-ev1
    $ make -f $PWD/../Makefile.sdk.##MACHINE## TF_M_CONFIG=m33td_nor-sdcard TF_M_PLATFORM=stm/stm32mp257f_ev1 TF_M_DEVICETREE=stm32mp257f-ev1-cm33tdcid-ostl  all

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
Update Starter Package with new FSBL binaries from <DEPLOYDIR> folder
    #> cp -rvf ${DEPLOYDIR}/* images/stm32mp*/arm-trusted-firmware-m/
        NB: if <DEPLOYDIR> has not been overriden at compilation step, use default path: <TF-M source code folder>/../deploy

Then the new Starter Package is ready to use for "Image flashing" on board (more information on wiki website https://wiki.st.com/stm32mpu).


----------------------------
8. Example of compilation usage
----------------------------
    $@E> cd ##BP##-##PR##
    $@E> tar xf ##BP##-##PR##.tar.xz
    $@E> cd ##BP##
    $@E> test -d .git || git init . && git add . && git commit -m "TF-M source code" && git gc

    $ cd ..
    $ cd ..
    $@P> cd ##BP##-##PR##
    $@P> cd ##BP##

    $@S> export FIP_DEPLOYDIR_ROOT=<your_deploy_dir_path>
    $@S> export TF_M_BUILD_PATH=$PWD/<your_build_subdir_path>

##CASE_stm32mp2-m33td##    "your_board_name" is like stm32mp257f-dk or stm32mp215f-mx-mycustomboard
##CASE_stm32mp2-m33td##    "your_storage_boot_scheme_cortex_m" is like m33td_emmc, m33td_nor, m33td_nor-emmc, m33td_nor-sdcard, m33td_sdcard
##CASE_stm32mp2-m33td##    "your_m33_profile" is like stm/stm32mp257f_ev1, stm/stm32mp215f-dk
##CASE_stm32mp2-m33td##    For runtime binaries
##CASE_stm32mp2-m33td##    $@C> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware-m TF_M_CONFIG=<your_storage_boot_scheme_cortex_m> TF_M_PLATFORM=<your_m33_profile> TF_M_DEVICETREE=<tfm_dt_name> EXTDT_DIR=<externaldt_path> EXTDT_DIR_TF_M=<externaldt_tfm_path> EXTDT_DIR_MCU=<externaldt_mcuboot_path>  tfm
##CASE_stm32mp2-m33td##      Example with external dt:
##CASE_stm32mp2-m33td##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware-m TF_M_CONFIG=m33td_sdcard TF_M_PLATFORM=stm/stm32mp215f_dk TF_M_DEVICETREE=stm32mp215f-dk-cm33tdcid-ostl   EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_M=cm33-td/tfm EXTDT_DIR_MCU=cm33-td/mcuboot tfm
##CASE_stm32mp2-m33td##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware-m TF_M_CONFIG=m33td_sdcard TF_M_PLATFORM=stm/stm32mp257f_ev1 TF_M_DEVICETREE=stm32mp257f-ev1-cm33tdcid-ostl EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_M=cm33-td/tfm EXTDT_DIR_MCU=cm33-td/mcuboot tfm
##CASE_stm32mp2-m33td##      Example with CubeMx devicetree:
##CASE_stm32mp2-m33td##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware-m TF_M_CONFIG=m33td_sdcard TF_M_PLATFORM=stm/stm32mp215f_dk TF_M_DEVICETREE=stm32mp215f-dk-cm33tdcid-ostl   EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_M=CM33/tfm EXTDT_DIR_MCU=CM33/DeviceTree/<your_cubemx_project_name>/mcuboot tfm
##CASE_stm32mp2-m33td##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware-m TF_M_CONFIG=m33td_sdcard TF_M_PLATFORM=stm/stm32mp257f_ev1 TF_M_DEVICETREE=stm32mp257f-ev1-cm33tdcid-ostl EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_M=CM33/tfm EXTDT_DIR_MCU=CM33/DeviceTree/<your_cubemx_project_name>/mcuboot tfm

