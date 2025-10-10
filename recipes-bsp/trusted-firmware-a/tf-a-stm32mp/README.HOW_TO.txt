Compilation of TF-A (Trusted Firmware-A):
1. Pre-requisite
2. Initialize cross-compilation via SDK
3. Prepare TF-A source code
4. Manage TF-A source code with GIT
5. Compile TF-A source code
6. Update software on board
7. Update starter package with TF-A compilation outputs

----------------
1. Pre-requisite
----------------
OpenSTLinux SDK must be installed.

For TF-A build you need to install:
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
3. Prepare TF-A source
----------------------
If not already done, extract the sources from Developer Package tarball, for example:
    $ tar xf en.SOURCES-stm32mp*-*.tar.xz

In the TF-A source directory (sources/*/##BP##-##PR##),
you have one TF-A source tarball, the patches and one Makefile:
   - ##BP##-##PR##.tar.xz
   - 00*.patch
   - Makefile.sdk.##MACHINE##

If you would like to have a git management for the source code move to
to section 4 [Management of TF-A source code with GIT].

Otherwise, to manage TF-A source code without git, you must extract the
tarball now and apply the patch:

    $> tar xf ##BP##-##PR##.tar.xz
    $> cd ##BP##
    $> for p in `ls -1 ../*.patch`; do patch -p1 < $p; done

You can now move to section 5 [Compile TF-A source code].

-----------------------------------
4. Manage TF-A source code with GIT
-----------------------------------
If you like to have a better management of change made on TF-A source, you
have 3 solutions to use git:

4.1 Get STMicroelectronics TF-A source from GitHub
--------------------------------------------------
    URL: https://github.com/STMicroelectronics/arm-trusted-firmware.git
    Branch: ##ARCHIVER_ST_BRANCH##
    Revision: ##ARCHIVER_ST_REVISION##

    $ git clone https://github.com/STMicroelectronics/arm-trusted-firmware.git
    $ git checkout -b WORKING ##ARCHIVER_ST_REVISION##

4.2 Create Git from tarball
---------------------------
    $ tar xf ##BP##-##PR##.tar.xz
    $ cd ##BP##
    $ test -d .git || git init . && git add . && git commit -m "tf-a source code" && git gc
    $ git checkout -b WORKING
    $ for p in `ls -1 ../*.patch`; do git am $p; done


4.3 Get Git from Arm Software community and apply STMicroelectronics patches
----------------------------------------------------------------------------
    URL: git://git.trustedfirmware.org/TF-A/trusted-firmware-a.git
    Branch: ##ARCHIVER_COMMUNITY_BRANCH##
    Revision: ##ARCHIVER_COMMUNITY_REVISION##

    $ git clone git://git.trustedfirmware.org/TF-A/trusted-firmware-a.git
    $ cd arm-trusted-firmware
    $ git checkout -b WORKING ##ARCHIVER_COMMUNITY_REVISION##
    $ for p in `ls -1 <path to patch>/*.patch`; do git am $p; done

---------------------------
5. Compile TF-A source code
---------------------------
According to your needs, there are 2 propositions to generate TF-A artifacts:

5.1 Updating Starter Package artifacts
--------------------------------------
If not already done, extract the artifacts from Starter Package tarball, for example:
    $ tar xf en.FLASH-##MACHINE##-*.tar.xz
Move to Starter Package root folder,
    $ cd <your_starter_package_dir_path>
Cleanup Starter Package from original TF-A artifacts first
    $ rm -rf images/##MACHINE##/arm-trusted-firmware/*
    $ rm -rf images/##MACHINE##/fip/*
Configure the DEPLOYDIR path to Starter Package TF-A artifacts folder
    $ export DEPLOYDIR=<your_starter_package_dir_path>/images/##MACHINE##/arm-trusted-firmware
Since OpenSTLinux activates FIP by default, FIP_artifacts directory path must be specified before launching compilation
    $ export FIP_DEPLOYDIR_ROOT=<your_starter_package_dir_path>/images/##MACHINE##

You can now move to section 5.3 [Generating TF-A artifacts].

5.2 Creating your own bootloader artifacts (development use case)
-----------------------------------------------------------------
With this configuration you will need to generate one by one all bootloader artifacts first before being able to generate
the FIP artifacts. And for that you need to share the same root folder for all bootloader compilation under Developer Package
Since OpenSTLinux activates FIP by default, FIP_artifacts directory path must be specified before launching compilation
    $> export FIP_DEPLOYDIR_ROOT=<bootloader artifacts location>
Make sure to configure then the DEPLOYDIR path accordingly:
    $> export DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware

You can now move to section 5.3 [Generating TF-A artifacts].

5.3 Generating TF-A artifacts
-----------------------------
To use the external device tree feature, EXTDT_DIR variable must be set to the root location of external DT
as specified in the README.HOW_TO.txt of external-dt
    $> export EXTDT_DIR=<external DT location>
To use the STM32MP DDR firmware, FWDDR_DIR variable must be set to the root location of STM32MP DDR firmware
as specified in the README.HOW_TO.txt of stm32mp-ddr-phy
    $> export FWDDR_DIR=<stm32mp ddr firmware location>

To list TF-A source code compilation configurations:
    $ cd <directory to TF-A source code>
    $ make -f $PWD/../Makefile.sdk.##MACHINE## help

There are different targets for TF-A compilation:

- Generate TF-A binaries
  Using default configuration, you just need to launch the 'stm32' target:
    $ make -f $PWD/../Makefile.sdk.##MACHINE## clean
    $ make -f $PWD/../Makefile.sdk.##MACHINE## stm32
  Example below for a specific config:
    $ make -f $PWD/../Makefile.sdk.##MACHINE## TF_A_DEVICETREE=stm32mp157f-ev1 TF_A_CONFIG=optee-sdcard ELF_DEBUG_ENABLE='1' clean
    $ make -f $PWD/../Makefile.sdk.##MACHINE## TF_A_DEVICETREE=stm32mp157f-ev1 TF_A_CONFIG=optee-sdcard ELF_DEBUG_ENABLE='1' stm32
        NB: TF_A_DEVICETREE flag must be set to switch to correct board configuration.
  The build results for this component are available in <DEPLOYDIR>.

- Generate TF-A metadata
  Using default configuration, you just need to launch the 'metadata' target:
    $ make -f $PWD/../Makefile.sdk.##MACHINE## clean
    $ make -f $PWD/../Makefile.sdk.##MACHINE## metadata
  Example below for a specific config:
    $ make -f $PWD/../Makefile.sdk.##MACHINE## TF_A_CONFIG=optee-sdcard clean
    $ make -f $PWD/../Makefile.sdk.##MACHINE## TF_A_CONFIG=optee-sdcard metadata
  The build results for this component are available in <DEPLOYDIR>.

- Generate FIP binaires
  Make sure to have all bootloader binaries (TF-A, U-Boot and optee-os) available in <FIP_DEPLOYDIR_ROOT> folder before launching the build
  Using default configuration, you just need to launch the 'fip' target:
    $ make -f $PWD/../Makefile.sdk.##MACHINE## fip
  Example below for a specific config:
    $ make -f $PWD/../Makefile.sdk.##MACHINE## TF_A_DEVICETREE=stm32mp157f-ev1 TF_A_CONFIG=optee-sdcard ELF_DEBUG_ENABLE='1' fip
        NB: TF_A_DEVICETREE flag must be set to switch to correct board configuration.
  The build results for this component are available in <FIP_DEPLOYDIR_ROOT>/fip

- Generate all TF-A artifacts in a row
  Make sure to have all other bootloader binaries (U-Boot and optee-os) available in <FIP_DEPLOYDIR_ROOT> folder before launching the build
  Using default configuration, you just need to launch the 'all' target:
    $> make -f $PWD/../Makefile.sdk.##MACHINE## clean
    $> make -f $PWD/../Makefile.sdk.##MACHINE## all
  Example below for a specific config:
    $ make -f $PWD/../Makefile.sdk.##MACHINE## TF_A_DEVICETREE=stm32mp157f-ev1 TF_A_CONFIG=optee-sdcard ELF_DEBUG_ENABLE='1' clean
    $ make -f $PWD/../Makefile.sdk.##MACHINE## TF_A_DEVICETREE=stm32mp157f-ev1 TF_A_CONFIG=optee-sdcard ELF_DEBUG_ENABLE='1' all
        NB: TF_A_DEVICETREE flag must be set to switch to correct board configuration.
  The build results for this component are available in <DEPLOYDIR> and <FIP_DEPLOYDIR_ROOT>/fip

---------------------------
6. Update software on board
---------------------------
Please use STM32CubeProgrammer to update the boot partitions, find more informations on the wiki website https://wiki.st.com/stm32mpu

-------------------------------------------------------------
7. Generate new Starter Package with TF-A compilation outputs
-------------------------------------------------------------
If not already done, extract the artifacts from Starter Package tarball, for example:
    # tar xf en.FLASH-stm32mp*-*.tar.xz

Move to Starter Package root folder,
    #> cd <your_starter_package_dir_path>
Cleanup Starter Package from original TF-A artifacts first
    #> rm -rf images/stm32mp*/arm-trusted-firmware/*
    #> rm -rf images/stm32mp*/fip/*
Update Starter Package with new FSBL binaries from <DEPLOYDIR> folder
    #> DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware && cp -rvf $DEPLOYDIR/* images/stm32mp*/arm-trusted-firmware/
        NB: if <DEPLOYDIR> has not been overriden at compilation step, use default path: <tf-a source code folder>/../deploy
Update Starter Package with new fip artifacts from <FIP_DEPLOYDIR_ROOT>/fip folder:
    #> cp -rvf $FIP_DEPLOYDIR_ROOT/fip/* images/stm32mp*/fip/

Then the new Starter Package is ready to use for "Image flashing" on board (more information on wiki website https://wiki.st.com/stm32mpu).

-------------------------------
8. Example of compilation usage
-------------------------------
    $@E> cd ##BP##-##PR##
    $@E> tar xf ##BP##-##PR##.tar.xz
    $@E> cd ##BP##
    $@E> for p in `ls -1 ../*.patch`; do patch -p1 < $p; done
    $@S> export FIP_DEPLOYDIR_ROOT=<your_deploy_dir_path>
    $ cd ..
    $ cd ..
    $@P> cd ##BP##-##PR##
    $@P> cd ##BP##


##CASE_stm32mp1##    "your_board_name" is like stm32mp157f-dk2 or stm32mp135f-mx-mycustomboard
##CASE_stm32mp1##    "your_storage_boot_scheme_cortex_a" is like optee-emmc, optee-sdcard, optee-nor, optee-nand, opteemin-emmc, opteemin-sdcard, opteemin-nor, opteemin-nand
##CASE_stm32mp1##    For runtime binaries
##CASE_stm32mp1##    $@C> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=<your_storage_boot_scheme_cortex_a> TF_A_DEVICETREE=<your_board_name> EXTDT_DIR=<externaldt_path> EXTDT_DIR_TF_A=<externaldt_tfa_path> metadata stm32
##CASE_stm32mp1##    For runtime FIP binaries
##CASE_stm32mp1##    $@F> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=<your_storage_boot_scheme_cortex_a> TF_A_DEVICETREE=<your_board_name> FIP_CONFIG=<your_storage_boot_scheme_cortex_a> EXTDT_DIR=<externaldt_path> EXTDT_DIR_TF_A=<externaldt_tfa_path> fip
##CASE_stm32mp1##    For flashing binaries
##CASE_stm32mp1##    $@PC> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb TF_A_DEVICETREE=<your_board_name> EXTDT_DIR=<externaldt_path> EXTDT_DIR_TF_A_SERIAL=<externaldt_tfa_programmer_path> stm32
##CASE_stm32mp1##    For flashing FIP binaries
##CASE_stm32mp1##    $@PF> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb TF_A_DEVICETREE=<your_board_name> EXTDT_DIR=<externaldt_path> FIP_CONFIG=optee-programmer-usb EXTDT_DIR_TF_A_SERIAL=<externaldt_tfa_programmer_path> fip
##CASE_stm32mp1##    Example with external dt:
##CASE_stm32mp1##      Example for runtime binaries
##CASE_stm32mp1##        $MP13> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-sdcard TF_A_DEVICETREE=stm32mp135f-dk EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A=ca7-td/tf-a metadata stm32
##CASE_stm32mp1##        $MP15> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-sdcard TF_A_DEVICETREE=stm32mp157f-dk2 EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A=ca7-td/tf-a metadata stm32
##CASE_stm32mp1##      Example for runtime FIP binaries
##CASE_stm32mp1##        $MP15> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-sdcard TF_A_DEVICETREE=stm32mp157f-dk2 EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A=ca7-td/tf-a fip
##CASE_stm32mp1##        $MP13> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-sdcard TF_A_DEVICETREE=stm32mp135f-dk  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A=ca7-td/tf-a fip
##CASE_stm32mp1##      Example for flashing binaries
##CASE_stm32mp1##        $MP15> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb    TF_A_DEVICETREE=stm32mp157f-dk2 EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A_SERIAL=ca7-td/tf-a stm32
##CASE_stm32mp1##        $MP13> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-serial TF_A_DEVICETREE=stm32mp135f-dk  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A_SERIAL=ca7-td/tf-a stm32
##CASE_stm32mp1##      Example for flashing FIP binaries
##CASE_stm32mp1##        $MP15> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb    TF_A_DEVICETREE=stm32mp157f-dk2 EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A_SERIAL=ca7-td/tf-a fip
##CASE_stm32mp1##        $MP13> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-serial TF_A_DEVICETREE=stm32mp135f-dk  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A_SERIAL=ca7-td/tf-a fip
##CASE_stm32mp1##    Example with CubeMx devicetree:
##CASE_stm32mp1##      Example for runtime binaries
##CASE_stm32mp1##        $MP1x> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-sdcard TF_A_DEVICETREE=<tfa_dt_programmer_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_A=DeviceTree/<your_cubemx_project_name>/tf-a metadata stm32
##CASE_stm32mp1##        $MP1x> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-emmc   TF_A_DEVICETREE=<tfa_dt_programmer_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_A=DeviceTree/<your_cubemx_project_name>/tf-a metadata stm32
##CASE_stm32mp1##      Example for runtime FIP binaries
##CASE_stm32mp1##        $MP1x> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-sdcard TF_A_DEVICETREE=<tfa_dt_programmer_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_A=DeviceTree/<your_cubemx_project_name>/tf-a fip
##CASE_stm32mp1##        $MP1x> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-emmc   TF_A_DEVICETREE=<tfa_dt_programmer_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_A=DeviceTree/<your_cubemx_project_name>/tf-a fip
##CASE_stm32mp1##      Example for flashing binaries
##CASE_stm32mp1##        $MP1x> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb    TF_A_DEVICETREE=<tfa_dt_programmer_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_A_SERIAL=DeviceTree/<your_cubemx_project_name>/tf-a stm32
##CASE_stm32mp1##        $MP1x> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-serial TF_A_DEVICETREE=<tfa_dt_programmer_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_A_SERIAL=DeviceTree/<your_cubemx_project_name>/tf-a stm32
##CASE_stm32mp1##      Example for flashing FIP binaries
##CASE_stm32mp1##        $MP1x> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb    TF_A_DEVICETREE=<tfa_dt_programmer_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_A_SERIAL=DeviceTree/<your_cubemx_project_name>/tf-a fip
##CASE_stm32mp1##        $MP1x> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-serial TF_A_DEVICETREE=<tfa_dt_programmer_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_A_SERIAL=DeviceTree/<your_cubemx_project_name>/tf-a fip

##CASE_stm32mp2##    "your_board_name" is like stm32mp257f-dk or stm32mp215f-mx-mycustomboard
##CASE_stm32mp2##    "your_storage_boot_scheme_cortex_a" is like optee-emmc, optee-sdcard, optee-nor, optee-nand, opteemin-emmc, opteemin-sdcard, opteemin-nor, opteemin-nand
##CASE_stm32mp2##    For runtime binaries
##CASE_stm32mp2##    $@C> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=<your_storage_boot_scheme_cortex_a> TF_A_DEVICETREE=<your_board_name> EXTDT_DIR=<externaldt_path> EXTDT_DIR_TF_A=<externaldt_tfa_path> metadata stm32
##CASE_stm32mp2##    For runtime FIP binaries
##CASE_stm32mp2##    $@F> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=<your_storage_boot_scheme_cortex_a> TF_A_DEVICETREE=<your_board_name> FIP_CONFIG=<your_storage_boot_scheme_cortex_a> EXTDT_DIR=<externaldt_path> EXTDT_DIR_TF_A=<externaldt_tfa_path> fip
##CASE_stm32mp2##    For flashing binaries
##CASE_stm32mp2##    $@PC> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb TF_A_DEVICETREE=<your_board_name> EXTDT_DIR=<externaldt_path> EXTDT_DIR_TF_A_SERIAL=<externaldt_tfa_programmer_path> stm32
##CASE_stm32mp2##    For flashing FIP binaries
##CASE_stm32mp2##    $@PF> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb TF_A_DEVICETREE=<your_board_name> EXTDT_DIR=<externaldt_path> FIP_CONFIG=optee-programmer-usb EXTDT_DIR_TF_A_SERIAL=<externaldt_tfa_programmer_path> fip
##CASE_stm32mp2##    Example with external dt:
##CASE_stm32mp2##      Example for runtime binaries
##CASE_stm32mp2##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-sdcard TF_A_DEVICETREE=stm32mp215f-dk  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A=ca35-td/tf-a metadata stm32
##CASE_stm32mp2##        $MP23> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-sdcard TF_A_DEVICETREE=stm32mp235f-dk  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A=ca35-td/tf-a metadata stm32
##CASE_stm32mp2##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-nor    TF_A_DEVICETREE=stm32mp257f-ev1 EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A=ca35-td/tf-a metadata stm32
##CASE_stm32mp2##      Example for runtime FIP binaries
##CASE_stm32mp2##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-sdcard TF_A_DEVICETREE=stm32mp215f-dk  FIP_CONFIG=optee-sdcard EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A=ca35-td/tf-a fip
##CASE_stm32mp2##        $MP23> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-sdcard TF_A_DEVICETREE=stm32mp235f-dk  FIP_CONFIG=optee-sdcard EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A=ca35-td/tf-a fip
##CASE_stm32mp2##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-nor    TF_A_DEVICETREE=stm32mp257f-ev1 FIP_CONFIG=optee-nor EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A=ca35-td/tf-afip
##CASE_stm32mp2##      Example for flashing binaries
##CASE_stm32mp2##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb TF_A_DEVICETREE=stm32mp215f-dk  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A_SERIAL=ca35-td/tf-a stm32
##CASE_stm32mp2##        $MP23> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb TF_A_DEVICETREE=stm32mp235f-dk  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A_SERIAL=ca35-td/tf-a stm32
##CASE_stm32mp2##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb TF_A_DEVICETREE=stm32mp257f-ev1 EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A_SERIAL=ca35-td/tf-a stm32
##CASE_stm32mp2##      Example for flashing FIP binaries
##CASE_stm32mp2##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb TF_A_DEVICETREE=stm32mp215f-dk  FIP_CONFIG=optee-programmer-usb EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A_SERIAL=ca35-td/tf-a fip
##CASE_stm32mp2##        $MP23> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb TF_A_DEVICETREE=stm32mp235f-dk  FIP_CONFIG=optee-programmer-usb EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A_SERIAL=ca35-td/tf-a fip
##CASE_stm32mp2##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb TF_A_DEVICETREE=stm32mp257f-ev1 FIP_CONFIG=optee-programmer-usb EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A_SERIAL=ca35-td/tf-a fip
##CASE_stm32mp2##    Example with CubeMx devicetree:
##CASE_stm32mp2##      Example for runtime binaries
##CASE_stm32mp2##        $MP2x> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-sdcard TF_A_DEVICETREE=<tfa_dt_programmer_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_A=CA35/DeviceTree/<your_cubemx_project_name>/tf-a metadata stm32
##CASE_stm32mp2##        $MP2x> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-emmc   TF_A_DEVICETREE=<tfa_dt_programmer_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_A=CA35/DeviceTree/<your_cubemx_project_name>/tf-a metadata stm32
##CASE_stm32mp2##      Example for runtime FIP binaries
##CASE_stm32mp2##        $MP2x> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-sdcard TF_A_DEVICETREE=<tfa_dt_programmer_name> EXTDT_DIR=<cubemx_output_dir> FIP_CONFIG=optee-sdcard EXTDT_DIR_TF_A=CA35/DeviceTree/<your_cubemx_project_name>/tf-a fip
##CASE_stm32mp2##        $MP2x> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-emmc   TF_A_DEVICETREE=<tfa_dt_programmer_name> EXTDT_DIR=<cubemx_output_dir> FIP_CONFIG=optee-emmc   EXTDT_DIR_TF_A=CA35/DeviceTree/<your_cubemx_project_name>/tf-a fip
##CASE_stm32mp2##      Example for flashing binaries
##CASE_stm32mp2##        $MP2x> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb    TF_A_DEVICETREE=<tfa_dt_programmer_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_A_SERIAL=CA35/DeviceTree/<your_cubemx_project_name>/tf-a stm32
##CASE_stm32mp2##        $MP2x> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-serial TF_A_DEVICETREE=<tfa_dt_programmer_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_A_SERIAL=CA35/DeviceTree/<your_cubemx_project_name>/tf-a stm32
##CASE_stm32mp2##      Example for flashing FIP binaries
##CASE_stm32mp2##        $MP2x> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb    TF_A_DEVICETREE=<tfa_dt_programmer_name> EXTDT_DIR=<cubemx_output_dir> FIP_CONFIG=optee-programmer-usb    EXTDT_DIR_TF_A_SERIAL=CA35/DeviceTree/<your_cubemx_project_name>/tf-a fip
##CASE_stm32mp2##        $MP2x> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-serial TF_A_DEVICETREE=<tfa_dt_programmer_name> EXTDT_DIR=<cubemx_output_dir> FIP_CONFIG=optee-programmer-serial EXTDT_DIR_TF_A_SERIAL=CA35/DeviceTree/<your_cubemx_project_name>/tf-a fip
##CASE_stm32mp2-m33td##    "your_board_name" is like stm32mp257f-dk or stm32mp215f-mx-mycustomboard
##CASE_stm32mp2-m33td##    "your_storage_boot_scheme_cortex_a" is like optee-emmc, optee-sdcard, optee-nor
##CASE_stm32mp2-m33td##    For runtime binaries
##CASE_stm32mp2-m33td##    $@C> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=<your_storage_boot_scheme_cortex_a> TF_A_DEVICETREE=<tfa_dt_name> EXTDT_DIR=<externaldt_path> EXTDT_DIR_TF_A=<externaldt_tfa_path> metadata stm32
##CASE_stm32mp2-m33td##    For runtime FIP binaries
##CASE_stm32mp2-m33td##    $@F> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=<your_storage_boot_scheme_cortex_a> TF_A_DEVICETREE=<tfa_dt_name> FIP_CONFIG=<your_storage_boot_scheme_cortex_a> EXTDT_DIR=<externaldt_path> EXTDT_DIR_TF_A=<externaldt_tfa_path> fip
##CASE_stm32mp2-m33td##    For flashing binaries
##CASE_stm32mp2-m33td##    $@PC> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb TF_A_DEVICETREE=<tfa_dt_programmer_name> EXTDT_DIR=<externaldt_path> EXTDT_DIR_TF_A_SERIAL=<externaldt_tfa_programmer_path> stm32
##CASE_stm32mp2-m33td##    For flashing FIP binaries
##CASE_stm32mp2-m33td##    $@PF> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb TF_A_DEVICETREE=<tfa_dt_programmer_name> FIP_CONFIG=optee-programmer-usb EXTDT_DIR=<externaldt_path> EXTDT_DIR_TF_A_SERIAL=<externaldt_tfa_programmer_path> fip
##CASE_stm32mp2-m33td##    Example with external dt:
##CASE_stm32mp2-m33td##      Example for runtime binaries
##CASE_stm32mp2-m33td##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-sdcard TF_A_DEVICETREE=stm32mp215f-dk-cm33tdcid-ostl EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A=cm33-td/tf-a metadata stm32
##CASE_stm32mp2-m33td##        $MP23> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-sdcard TF_A_DEVICETREE=stm32mp235f-dk-cm33tdcid-ostl EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A=cm33-td/tf-a metadata stm32
##CASE_stm32mp2-m33td##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-emmc   TF_A_DEVICETREE=stm32mp257f-ev1-cm33tdcid-ostl  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A=cm33-td/tf-a metadata stm32
##CASE_stm32mp2-m33td##      Example for runtime FIP binaries
##CASE_stm32mp2-m33td##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-sdcard TF_A_DEVICETREE=stm32mp215f-dk-cm33tdcid-ostl  FIP_CONFIG=optee-sdcard EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A=cm33-td/tf-a fip
##CASE_stm32mp2-m33td##        $MP23> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-sdcard TF_A_DEVICETREE=stm32mp235f-dk-cm33tdcid-ostl  FIP_CONFIG=optee-sdcard EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A=cm33-td/tf-a fip
##CASE_stm32mp2-m33td##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-emmc   TF_A_DEVICETREE=stm32mp257f-ev1-cm33tdcid-ostl FIP_CONFIG=optee-emmc EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A=cm33-td/tf-a fip
##CASE_stm32mp2-m33td##      Example for flashing binaries
##CASE_stm32mp2-m33td##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb TF_A_DEVICETREE=stm32mp215f-dk-cm33tdcid-ostl-serial-ca35tdcid EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A_SERIAL=cm33-td/tf-a stm32
##CASE_stm32mp2-m33td##        $MP23> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb TF_A_DEVICETREE=stm32mp235f-dk-cm33tdcid-ostl-serial-ca35tdcid EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A_SERIAL=cm33-td/tf-a stm32
##CASE_stm32mp2-m33td##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb TF_A_DEVICETREE=stm32mp257f-ev1-cm33tdcid-ostl-serial-ca35tdcid EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A_SERIAL=cm33-td/tf-a stm32
##CASE_stm32mp2-m33td##      Example for flashing FIP binaries
##CASE_stm32mp2-m33td##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb TF_A_DEVICETREE=stm32mp215f-dk-cm33tdcid-ostl-serial-ca35tdcid  FIP_CONFIG=optee-programmer-usb EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A_SERIAL=cm33-td/tf-a fip
##CASE_stm32mp2-m33td##        $MP23> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb TF_A_DEVICETREE=stm32mp235f-dk-cm33tdcid-ostl-serial-ca35tdcid  FIP_CONFIG=optee-programmer-usb EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A_SERIAL=cm33-td/tf-a fip
##CASE_stm32mp2-m33td##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb TF_A_DEVICETREE=stm32mp257f-ev1-cm33tdcid-ostl-serial-ca35tdcid FIP_CONFIG=optee-programmer-usb EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_TF_A_SERIAL=cm33-td/tf-a fip
##CASE_stm32mp2-m33td##    Example with CubeMx devicetree:
##CASE_stm32mp2-m33td##      Example for runtime binaries
##CASE_stm32mp2-m33td##        $MP2x> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-emmc   TF_A_DEVICETREE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_A=CA35/DeviceTree/<your_cubemx_project_name>/tf-a metadata stm32
##CASE_stm32mp2-m33td##        $MP2x> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-sdcard TF_A_DEVICETREE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_A=CA35/DeviceTree/<your_cubemx_project_name>/tf-a metadata stm32
##CASE_stm32mp2-m33td##      Example for runtime FIP binaries
##CASE_stm32mp2-m33td##        $MP2x> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-emmc   TF_A_DEVICETREE=<your_CUBE_MX_board_name> FIP_CONFIG=optee-emmc EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_A=CA35/DeviceTree/<your_cubemx_project_name>/tf-a fip
##CASE_stm32mp2-m33td##        $MP2x> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-sdcard TF_A_DEVICETREE=<your_CUBE_MX_board_name> FIP_CONFIG=optee-sdcard EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_A=CA35/DeviceTree/<your_cubemx_project_name>/tf-a fip
##CASE_stm32mp2-m33td##      Example for flashing binaries
##CASE_stm32mp2-m33td##        $MP2x> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb    TF_A_DEVICETREE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_A_SERIAL=ExtMemLoader/DeviceTree/<your_cubemx_project_name>/tf-a stm32
##CASE_stm32mp2-m33td##        $MP2x> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-serial TF_A_DEVICETREE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_A_SERIAL=ExtMemLoader/DeviceTree/<your_cubemx_project_name>/tf-a stm32
##CASE_stm32mp2-m33td##      Example for flashing FIP binaries
##CASE_stm32mp2-m33td##        $MP2x> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-usb    TF_A_DEVICETREE=<your_CUBE_MX_board_name> FIP_CONFIG=optee-programmer-usb EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_A_SERIAL=ExtMemLoader/DeviceTree/<your_cubemx_project_name>/tf-a fip
##CASE_stm32mp2-m33td##        $MP2x> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware TF_A_CONFIG=optee-programmer-serial TF_A_DEVICETREE=<your_CUBE_MX_board_name> FIP_CONFIG=optee-programmer-serial EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_TF_A_SERIAL=ExtMemLoader/DeviceTree/<your_cubemx_project_name>/tf-a fip
