Compilation of Optee-os (Trusted Execution Environment):
1. Pre-requisite
2. Initialize cross-compilation via SDK
3. Prepare optee-os source code
4. Manage optee-os source code
5. Compile optee-os source code
6. Update software on board
7. Update starter package with optee-os compilation outputs
8. Example of compilation usage

----------------
1. Pre-requisite
----------------
OpenSTLinux SDK must be installed.

If you have never configured you git configuration:
    $ git config --global user.name "your_name"
    $ git config --global user.email "your_email@example.com"

---------------------------------------
2. Initialize cross-compilation via SDK
---------------------------------------
Source SDK environment:
    $ source <path to SDK>/environment-setup

* To verify that your cross-compilation environment is set-up correctly:
    $ set | grep CROSS_COMPILE

  If the variable CROSS_COMPILE has a value:
   - arm-ostl-linux-gnueabi- for 32 bits architecture (for example STM32MP1)
   - aarch64-ostl-linux- for 64 bits architecture (for example STM32MP2)
  Then everything is set-up correctly

Warning: the environment are valid only on the shell session where you have
 sourced the sdk environment.

External device tree is extracted. If this is not the case, please follow the
README_HOW_TO.txt in ../external-dt.

--------------------------
3. Prepare optee-os source
--------------------------
If not already done, extract the sources from Developer Package tarball, for example:
    $ tar xf en.SOURCES-stm32mp*-*.tar.xz

In the optee-os source directory (sources/*/##BP##-##PR##),
you have one optee-os source tarball, the patches and one Makefile:
   - ##BP##-##PR##.tar.xz
   - 00*.patch
   - Makefile.sdk.##MACHINE##

If you would like to have a git management for the source code move to
to section 4 [Management of optee-os source code with GIT].

Otherwise, to manage optee-os source code without git, you must extract the
tarball now and apply the patch:

    $> tar xf ##BP##-##PR##.tar.xz
    $> cd ##BP##
    $> tar xf ../fonts.tar.gz
    $> for p in `ls -1 ../*.patch`; do patch -p1 < $p; done

You can now move to section 5 [Compile optee-os source code].

---------------------------------------
4. Manage optee-os source code with GIT
---------------------------------------
If you like to have a better management of change made on optee-os source,
you have 3 solutions to use git

4.1 Get STMicroelectronics optee-os source from GitHub
------------------------------------------------------
    URL: https://github.com/STMicroelectronics/optee_os.git
    Branch: ##ARCHIVER_ST_BRANCH##
    Revision: ##ARCHIVER_ST_REVISION##

    $ git clone https://github.com/STMicroelectronics/optee_os.git
    $ git checkout -b WORKING ##ARCHIVER_ST_REVISION##

4.2 Create Git from tarball
---------------------------
    $ tar xf ##BP##-##PR##.tar.xz
    $ cd ##BP##
    $ test -d .git || git init . && git add . && git commit -m "optee-os source code" && git gc
    $ git checkout -b WORKING
    $ tar xf ../fonts.tar.gz
    $ for p in `ls -1 ../*.patch`; do git am $p; done

MANDATORY: You must update sources
    $ cd <directory to optee-os source code>
    $ chmod 755 scripts/bin_to_c.py

4.3 Get Git from community and apply STMicroelectronics patches
---------------------------------------------------------------
* With the optee-os source code from the OP-TEE git repositories:
    URL: git://github.com/OP-TEE/optee_os.git
    Branch: ##ARCHIVER_COMMUNITY_BRANCH##
    Revision: ##ARCHIVER_COMMUNITY_REVISION##

    $ git clone git://github.com/OP-TEE/optee_os.git
    $ cd optee_os
    $ git checkout -b WORKING ##ARCHIVER_COMMUNITY_REVISION##
    $ tar xf <path to patch>/fonts.tar.gz
    $ for p in `ls -1 <path to patch>/*.patch`; do git am $p; done

MANDATORY: You must update sources
    $ cd <directory to optee-os source code>
    $ chmod 755 scripts/bin_to_c.py

-------------------------------
5. Compile optee-os source code
-------------------------------
According to your needs, there are 2 propositions to generate optee-os artifacts:

5.1 Updating Starter Package artifacts
--------------------------------------
If not already done, extract the artifacts from Starter Package tarball, for example:
    $ tar xf en.FLASH-##MACHINE##-*.tar.xz
Move to Starter Package root folder,
    $ cd <your_starter_package_dir_path>
Cleanup Starter Package from original optee-os artifacts first
    $ rm -rf images/##MACHINE##/optee/*
    $ rm -rf images/##MACHINE##/fip/*
Configure the DEPLOYDIR path to Starter Package optee-os artifacts folder
    $ export DEPLOYDIR=<your_starter_package_dir_path>/images/##MACHINE##/optee
Since OpenSTLinux activates FIP by default, FIP_artifacts directory path must be specified before launching compilation
    $ export FIP_DEPLOYDIR_ROOT=<your_starter_package_dir_path>/images/##MACHINE##

You can now move to section 5.3 [Generating optee-os artifacts].

5.2 Creating your own bootloader artifacts (development use case)
-----------------------------------------------------------------
With this configuration you will need to generate one by one all bootloader artifacts first before being able to generate
the FIP artifacts. And for that you need to share the same root folder for all bootloader compilation under Developer Package
Since OpenSTLinux activates FIP by default, FIP_artifacts directory path must be specified before launching compilation
    $> export FIP_DEPLOYDIR_ROOT=<bootloader artifacts location>
Make sure to configure then the DEPLOYDIR path accordingly:
    $> export DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee

You can now move to section 5.3 [Generating optee-os artifacts].

5.3 Generating optee-os artifacts
---------------------------------
To use the external device tree feature, EXTDT_DIR variable must be set to the root location of external DT
as specified in the README.HOW_TO.txt of external-dt
    $> export EXTDT_DIR=<external DT location>

To list optee-os source code compilation configurations:
    $ cd <directory to optee-os source code>
    $ make -f $PWD/../Makefile.sdk.##MACHINE## help

There are different targets for optee-os compilation:

- Generate optee-os binaries
  Using default configuration, you just need to launch the 'optee' target:
    $ make -f $PWD/../Makefile.sdk.##MACHINE## clean
    $ make -f $PWD/../Makefile.sdk.##MACHINE## optee
  Example below for a specific config:
    $ make -f $PWD/../Makefile.sdk.##MACHINE## CFG_EMBED_DTB_SOURCE_FILE=stm32mp157c-ev1 clean
    $ make -f $PWD/../Makefile.sdk.##MACHINE## CFG_EMBED_DTB_SOURCE_FILE=stm32mp157c-ev1 optee
  The build results for this component are available in <DEPLOYDIR>.

- Generate FIP binaires
  Make sure to have all bootloader binaries (TF-A, U-Boot and optee-os) available in <FIP_DEPLOYDIR_ROOT> folder before launching the build
  Using default configuration, you just need to launch the 'fip' target:
    $ make -f $PWD/../Makefile.sdk.##MACHINE## fip
  Example below for a specific config:
    $ make -f $PWD/../Makefile.sdk.##MACHINE## CFG_EMBED_DTB_SOURCE_FILE=stm32mp157c-ev1 fip
  The build results for this component are available in <FIP_DEPLOYDIR_ROOT>/fip

- Generate all optee-os artifacts in a row
  Make sure to have all other bootloader binaries (TF-A and U-Boot) available in <FIP_DEPLOYDIR_ROOT> folder before launching the build
  Using default configuration, you just need to launch the 'all' target:
    $> make -f $PWD/../Makefile.sdk.##MACHINE## clean
    $> make -f $PWD/../Makefile.sdk.##MACHINE## all
  Example below for a specific config:
    $ make -f $PWD/../Makefile.sdk.##MACHINE## CFG_EMBED_DTB_SOURCE_FILE=stm32mp157c-ev1 clean
    $ make -f $PWD/../Makefile.sdk.##MACHINE## CFG_EMBED_DTB_SOURCE_FILE=stm32mp157c-ev1 all
  The build results for this component are available in <DEPLOYDIR> and <FIP_DEPLOYDIR_ROOT>/fip

---------------------------
6. Update software on board
---------------------------
Please use STM32CubeProgrammer and only tick the ssbl-boot and fip partitions (more informations on the wiki website http://wiki.st.com/stm32mpu)

-----------------------------------------------------------------
7. Generate new Starter Package with optee-os compilation outputs
-----------------------------------------------------------------
If not already done, extract the artifacts from Starter Package tarball, for example:
    # tar xf en.FLASH-stm32mp*-*.tar.xz

Move to Starter Package root folder,
    #> cd <your_starter_package_dir_path>
Cleanup Starter Package from original optee-os artifacts first
    #> rm -rf images/stm32mp*/fip/*
Update Starter Package with new fip artifacts from <FIP_DEPLOYDIR_ROOT>/fip folder:
    #> cp -rvf $FIP_DEPLOYDIR_ROOT/fip/* images/stm32mp*/fip/

Then the new Starter Package is ready to use for "Image flashing" on board (more information on wiki website https://wiki.st.com/stm32mpu).

-------------------------------
8. Example of compilation usage
-------------------------------
    $@E> cd ##BP##-##PR##
    $@E> tar xf ##BP##-##PR##.tar.xz
    $@E> cd ##BP##
    $@E> tar xf ../fonts.tar.gz
    $@E> for p in `ls -1 ../*.patch`; do patch -p1 < $p; done
    $@S> export FIP_DEPLOYDIR_ROOT=<your_deploy_dir_path>
    $ cd ..
    $ cd ..
    $@P> cd ##BP##-##PR##
    $@P> cd ##BP##

##CASE_stm32mp1##    "your_board_name" is like stm32mp157f-dk2 or stm32mp135f-mx-mycustomboard
##CASE_stm32mp1##    "your_storage_boot_scheme_security" is like optee or opteemin or optee-programmer or opteemin-programmer
##CASE_stm32mp1##    For runtime binaries
##CASE_stm32mp1##    $@C> make -f $PWD/../Makefile.sdk.stm32mp1 OPTEE_CONFIG=<your_storage_boot_scheme_security> CFG_EMBED_DTB_SOURCE_FILE=<your_board_name> DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee EXTDT_DIR=<externaldt_path> EXTDT_DIR_OPTEE=<externaldt_optee_path> optee
##CASE_stm32mp1##    For flashing binaries
##CASE_stm32mp1##    $@PC> make -f $PWD/../Makefile.sdk.stm32mp1 OPTEE_CONFIG=optee-programmer CFG_EMBED_DTB_SOURCE_FILE=<your_board_name> DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee EXTDT_DIR=<externaldt_path> EXTDT_DIR_OPTEE_SERIAL=<externaldt_optee_path> optee
##CASE_stm32mp1##    Example with external dt:
##CASE_stm32mp1##      Example for runtime binaries
##CASE_stm32mp1##        $MP13> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=optee    CFG_EMBED_DTB_SOURCE_FILE=stm32mp135f-dk  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_OPTEE=ca7-td/optee optee
##CASE_stm32mp1##        $MP15> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=opteemin CFG_EMBED_DTB_SOURCE_FILE=stm32mp157f-ev1 EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_OPTEE=ca7-td/optee optee
##CASE_stm32mp1##      Example for flashing binaries
##CASE_stm32mp1##        $MP13> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=optee-programmer    CFG_EMBED_DTB_SOURCE_FILE=stm32mp135f-dk  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_OPTEE_SERIAL=ca7-td/optee optee
##CASE_stm32mp1##        $MP15> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=opteemin-programmer CFG_EMBED_DTB_SOURCE_FILE=stm32mp157f-ev1 EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_OPTEE_SERIAL=ca7-td/optee optee
##CASE_stm32mp1##    Example with CubeMx devicetree:
##CASE_stm32mp1##      Example for runtime binaries
##CASE_stm32mp1##        $MP1x> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=optee    CFG_EMBED_DTB_SOURCE_FILE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_OPTEE=CA7/DeviceTree/<your_cubemx_project_name>/optee optee
##CASE_stm32mp1##        $MP1x> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=opteemin CFG_EMBED_DTB_SOURCE_FILE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_OPTEE=CA7/DeviceTree/<your_cubemx_project_name>/optee optee
##CASE_stm32mp1##      Example for flashing binaries
##CASE_stm32mp1##        $MP1x> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=optee-programmer    CFG_EMBED_DTB_SOURCE_FILE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_OPTEE_SERIAL=CA7/DeviceTree/<your_cubemx_project_name>/optee optee
##CASE_stm32mp1##        $MP1x> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=opteemin-programmer CFG_EMBED_DTB_SOURCE_FILE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_OPTEE_SERIAL=CA7/DeviceTree/<your_cubemx_project_name>/optee optee
##CASE_stm32mp2##    "your_board_name" is like stm32mp257f-dk or stm32mp215f-mx-mycustomboard
##CASE_stm32mp2##    "your_storage_boot_scheme_security" is like optee or opteemin
##CASE_stm32mp2##    For runtime binaries
##CASE_stm32mp2##    $@C> make -f $PWD/../Makefile.sdk.stm32mp2 OPTEE_CONFIG=<your_storage_boot_scheme_security> CFG_EMBED_DTB_SOURCE_FILE=<your_board_name> DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee EXTDT_DIR=<externaldt_path> EXTDT_DIR_OPTEE=<externaldt_optee_path>  optee
##CASE_stm32mp2##    For flashing binaries
##CASE_stm32mp2##    $@PC> make -f $PWD/../Makefile.sdk.stm32mp2 OPTEE_CONFIG=optee-programmer CFG_EMBED_DTB_SOURCE_FILE=<your_board_name> DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee EXTDT_DIR=<externaldt_path> EXTDT_DIR_OPTEE=<externaldt_optee_path>  optee
##CASE_stm32mp2##    Support of both type of TA: TA_arm64 and TA_arm32
##CASE_stm32mp2##    $ make -f $PWD/../Makefile.sdk.stm32mp2 OPTEE_CONFIG=optee TA32_64=1 CFG_EMBED_DTB_SOURCE_FILE=<your_board_name> DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee optee
##CASE_stm32mp2##    Example with external dt:
##CASE_stm32mp2##      Example for runtime binaries
##CASE_stm32mp2##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=optee CFG_EMBED_DTB_SOURCE_FILE=stm32mp215-dk   EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_OPTEE=ca35-td/optee optee
##CASE_stm32mp2##        $MP23> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=optee CFG_EMBED_DTB_SOURCE_FILE=stm32mp235f-dk  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_OPTEE=ca35-td/optee optee
##CASE_stm32mp2##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=optee CFG_EMBED_DTB_SOURCE_FILE=stm32mp257f-ev1 EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_OPTEE=ca35-td/optee optee
##CASE_stm32mp2##      Example for flashing binaries
##CASE_stm32mp2##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=optee-programmer CFG_EMBED_DTB_SOURCE_FILE=stm32mp215-dk   EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_OPTEE_SERIAL=ca35-td/optee optee
##CASE_stm32mp2##        $MP23> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=optee-programmer CFG_EMBED_DTB_SOURCE_FILE=stm32mp235f-dk  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_OPTEE_SERIAL=ca35-td/optee optee
##CASE_stm32mp2##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=optee-programmer CFG_EMBED_DTB_SOURCE_FILE=stm32mp257f-ev1 EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_OPTEE_SERIAL=ca35-td/optee optee
##CASE_stm32mp2##    Example with CubeMx devicetree:
##CASE_stm32mp2##      Example for runtime binaries
##CASE_stm32mp2##        $MP2x> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=optee CFG_EMBED_DTB_SOURCE_FILE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_OPTEE=CA35/DeviceTree/<your_cubemx_project_name>/optee optee
##CASE_stm32mp2##      Example for flashing binaries
##CASE_stm32mp2##        $MP2x> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=optee-programmer CFG_EMBED_DTB_SOURCE_FILE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_OPTEE_SERIAL=CA35/DeviceTree/<your_cubemx_project_name>/optee optee
##CASE_stm32mp2-m33td##    "your_board_name" is like stm32mp257f-dk or stm32mp215f-mx-mycustomboard
##CASE_stm32mp2-m33td##    For runtime binaries
##CASE_stm32mp2-m33td##    $@C> make -f $PWD/../Makefile.sdk.stm32mp2-m33td OPTEE_CONFIG=optee CFG_EMBED_DTB_SOURCE_FILE=<optee_dt_name> EXTDT_DIR=<externaldt_path> EXTDT_DIR_OPTEE=<externaldt_optee_path> DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee optee
##CASE_stm32mp2-m33td##    For flashing binaries
##CASE_stm32mp2-m33td##    $@PC> make -f $PWD/../Makefile.sdk.stm32mp2-m33td OPTEE_CONFIG=optee-programmer CFG_EMBED_DTB_SOURCE_FILE=<optee_dt_programmer_name> EXTDT_DIR=<externaldt_path> EXTDT_DIR_OPTEE_SERIAL=<externaldt_optee_programmer_path> DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee optee
##CASE_stm32mp2-m33td##    Example with external dt:
##CASE_stm32mp2-m33td##      Example for runtime binaries
##CASE_stm32mp2-m33td##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=optee CFG_EMBED_DTB_SOURCE_FILE=stm32mp215-dk-cm33tdcid-ostl-sdcard   EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_OPTEE=cm33-td/optee optee
##CASE_stm32mp2-m33td##        $MP23> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=optee CFG_EMBED_DTB_SOURCE_FILE=stm32mp235f-dk-cm33tdcid-ostl-sdcard  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_OPTEE=cm33-td/optee optee
##CASE_stm32mp2-m33td##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=optee CFG_EMBED_DTB_SOURCE_FILE=stm32mp257f-ev1-cm33tdcid-ostl-emmc   EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_OPTEE=cm33-td/optee optee
##CASE_stm32mp2-m33td##      Example for flashing binaries
##CASE_stm32mp2-m33td##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=optee-programmer CFG_EMBED_DTB_SOURCE_FILE=stm32mp215-dk-cm33tdcid-ostl-serial-ca35tdcid   EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_OPTEE_SERIAL=cm33-td/optee optee
##CASE_stm32mp2-m33td##        $MP23> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=optee-programmer CFG_EMBED_DTB_SOURCE_FILE=stm32mp235f-dk-cm33tdcid-ostl-serial-ca35tdcid  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_OPTEE_SERIAL=cm33-td/optee optee
##CASE_stm32mp2-m33td##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=optee-programmer CFG_EMBED_DTB_SOURCE_FILE=stm32mp257f-ev1-cm33tdcid-ostl-serial-ca35tdcid EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_OPTEE_SERIAL=cm33-td/optee optee
##CASE_stm32mp2-m33td##    Example with CubeMx devicetree:
##CASE_stm32mp2-m33td##      Example for runtime binaries
##CASE_stm32mp2-m33td##        $MP2x> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=optee CFG_EMBED_DTB_SOURCE_FILE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_OPTEE=CA35/DeviceTree/<your_cubemx_project_name>/optee optee
##CASE_stm32mp2-m33td##      Example for flashing binaries
##CASE_stm32mp2-m33td##        $MP2x> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee OPTEE_CONFIG=optee-programmer CFG_EMBED_DTB_SOURCE_FILE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_OPTEE_SERIAL=Extern/DeviceTree/<your_cubemx_project_name>/optee optee

