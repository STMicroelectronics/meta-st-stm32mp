Compilation of CubeMp2 (m33tdprojects Starter stm32mp2):
1. Pre-requisite
2. Initialize cross-compilation via SDK
3. Prepare CubeMp2source code
4. Manage CubeMp2 source code with GIT
5. Compile CubeMp2 source code
6. Update software on board
7. Update starter package with CubeMp2 compilation outputs

----------------
1. Pre-requisite
----------------
OpenSTLinux SDK must be installed.

For CubeMp2 build you need to install:
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
   - aarch64-ostl-linux- for 64 bits architecture (for example STM32MP2)
  Then everything is set-up correctly

Warning: the environment are valid only on the shell session where you have
sourced the sdk environment.

-------------------------
3. Prepare CubeMp2 source
-------------------------
If not already done, extract the sources from Developer Package tarball, for example:
    $ tar xf en.SOURCES-stm32mp*-*.tar.xz

In the CubeMp2 source directory (sources/*/##BP##-##PR##),
you have one CubeMp2 source tarball, the patches and one Makefile:
   - ##BP##-##PR##.tar.xz
   - 00*.patch
   - Makefile.sdk.##MACHINE##

If you would like to have a git management for the source code move to
to section 4 [Management of CubeMp2 source code with GIT].

Otherwise, to manage CubeMp2 source code without git, you must extract the
tarball now:

    $> tar xf ##BP##-##PR##.tar.xz
    $> cd ##BP##
    $> for p in `ls -1 ../*.patch`; do patch -p1 < $p; done

You can now move to section 5 [Compile CubeMp2 source code].

--------------------------------------
4. Manage CubeMp2 source code with GIT
--------------------------------------
If you like to have a better management of change made on CubeMp2 source, you
have the solution to use git:

4.1 Get STMicroelectronics CubeMp2 source from GitHub
--------------------------------------------------
    URL: https://github.com/STMicroelectronics/STM32CubeMP2
    Branch: ##ARCHIVER_ST_BRANCH##
    Revision: ##ARCHIVER_ST_REVISION##

    $ git clone https://github.com/STMicroelectronics/STM32CubeMP2
    $ git checkout -b WORKING ##ARCHIVER_ST_REVISION##

------------------------------
5. Compile CubeMp2 source code
------------------------------
According to your needs, there are 2 propositions to generate CubeMp2 artifacts:

5.1 Updating Starter Package artifacts
--------------------------------------
If not already done, extract the artifacts from Starter Package tarball, for example:
    $ tar xf en.FLASH-##MACHINE##-*.tar.xz
Move to Starter Package root folder,
    $ cd <your_starter_package_dir_path>
Cleanup Starter Package from original CubeMp2 artifacts first
    $ rm -rf images/##MACHINE##/m33-projects/*
    $ rm -rf images/##MACHINE##/m33-firmware/*
Configure the DEPLOYDIR path to Starter Package CubeMp2 artifacts folder
    $ export DEPLOYDIR=<your_starter_package_dir_path>/images/##MACHINE##/m33-projects
The M33FW_artifacts directory path must be specified before launching compilation
    $ export M33FW_DEPLOYDIR_ROOT=<your_starter_package_dir_path>/images/##MACHINE##

You can now move to section 5.3 [Generating CubeMp2 artifacts].

5.2 Creating your own M33FW artifacts (development use case)
-----------------------------------------------------------------
With this configuration you will need to generate one by one the DDR, non-secure and secure firmware artifacts first before being able to generate
the M33FW artifacts. And for that you need to share the same root folder for tf-m and m33tdprojects compilation under Developer Package
The M33FW_artifacts directory path must be specified before launching compilation
    $> export M33FW_DEPLOYDIR_ROOT=<tf-m and m33tdprojects artifacts location>
Make sure to configure then the DEPLOYDIR path accordingly:
    $> export DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/m33-projects

You can now move to section 5.3 [Generating CubeMp2 artifacts].

5.3 Generating CubeMp2 artifacts
-----------------------------
To use the external device tree feature, EXTDT_DIR variable must be set to the root location of external DT
as specified in the README.HOW_TO.txt of external-dt
    $> export EXTDT_DIR=<external DT location>
For M33TD build, the TF-M build directory path is mandatory to generate CubeMp2 firmware
    $> export TF_M_BUILD_PATH=<tf-m-stm32mp build dir>

The build results for this component are available in DEPLOYDIR (Default: $PWD/../deploy).
If needed, this deploy directory can be specified by adding "DEPLOYDIR=<your_deploy_dir_path>" compilation option to the build command line below.

For example configure a dedicated deploy directory at the same level of m33tdprojects source code:
    $ cd ##BP##
    $ for p in `ls -1 ../*.patch`; do git am $p; done
    $ export DEPLOYDIR=$PWD/../../deploy/m33-projects
    $> mkdir -p ${DEPLOYDIR}

To list CubeMp2 source code compilation configurations:
    $> make -j 1 -f $PWD/../Makefile.sdk.##MACHINE## help

There are different targets for CubeMp2 compilation:

- Generate CubeMp2 binaries
  Using default configuration, you just need to launch the 'm33td' target:
    $> make -j 1 -f $PWD/../Makefile.sdk.##MACHINE## clean
    $> make -j 1 -f $PWD/../Makefile.sdk.##MACHINE## m33td
  Example below for a specific config:
    $ make -f $PWD/../Makefile.sdk.##MACHINE## DEPLOYDIR=$PWD/../deploy M33FW_CONFIG=m33td_sdcard M33FW_DEVICETREE=<device tree> clean
    $ make -f $PWD/../Makefile.sdk.##MACHINE## DEPLOYDIR=$PWD/../deploy M33FW_CONFIG=m33td_sdcard M33FW_DEVICETREE=<device tree> m33td
  The build results for this component are available in <DEPLOYDIR>.

- Generate M33FW binaires
  Make sure to have all m33 firmwares binaries (CubeMp2 and TF-M) available in <M33FW_DEPLOYDIR_ROOT> folder before launching the build
  Using default configuration, you just need to launch the 'm33fw' target:
    $> make -j 1 -f $PWD/../Makefile.sdk.##MACHINE## m33fw
  Example below for a specific config:
    $ make -f $PWD/../Makefile.sdk.##MACHINE## DEPLOYDIR=$PWD/../deploy M33FW_CONFIG=m33td_sdcard M33FW_DEVICETREE=<device tree> m33fw
  The build results for this component are available in <M33FW_DEPLOYDIR_ROOT>/m33-firmware

---------------------------
6. Update software on board
---------------------------
Please use STM32CubeProgrammer to update the m33fw-a partitions, find more informations on the wiki website https://wiki.st.com/stm32mpu
The binary generated by CubeMp2 on M33TD configuration provide the firmware to be executed at startup by M33 processor.

----------------------------------------------------------
7. Update starter package with CubeMp2 compilation outputs
----------------------------------------------------------
If not already done, extract the artifacts from Starter Package tarball, for example:
    # tar xf en.FLASH-stm32mp*-*.tar.xz

Move to Starter Package root folder,
    #> cd <your_starter_package_dir_path>
Cleanup Starter Package from original CubeMp2 artifacts first
    #> rm -rf images/stm32mp*/m33-projects/*
    #> rm -rf images/stm32mp*/m33-firmware/*
Update Starter Package with new CubeMp2 binaries from <DEPLOYDIR> folder
    #> cp -rvf ${DEPLOYDIR}/* images/stm32mp*/m33-projects/
        NB: if <DEPLOYDIR> has not been overriden at compilation step, use default path: <CubeMp2 source code folder>/../deploy
Update Starter Package with new m33-firmware artifacts from <M33FW_DEPLOYDIR_ROOT>/m33-firmware folder:
    #> cp -rvf $M33FW_DEPLOYDIR_ROOT/m33-firmware/* images/stm32mp*/m33-firmware/

Then the new Starter Package is ready to use for "Image flashing" on board (more information on wiki website https://wiki.st.com/stm32mpu).


---------------------------
8. Example of compilation usage
---------------------------
    $@E> cd ##BP##-##PR##
    $@E> tar xf ##BP##-##PR##.tar.xz
    $@E> cd ##BP##
    $@E> for p in `ls -1 ../*.patch`; do patch -p1 < $p; done

    $ cd ..
    $ cd ..
    $@P> cd ##BP##-##PR##
    $@P> cd ##BP##

    $@S> export M33FW_DEPLOYDIR_ROOT=<your_deploy_dir_path>

##CASE_stm32mp2-m33td##    "your_board_name" is like stm32mp157f-dk2 or stm32mp135f-mx-mycustomboard
##CASE_stm32mp2-m33td##    "your_storage_boot_scheme_cortex_m" is like m33td_emmc, m33td_nor, m33td_nor-emmc, m33td_nor-sdcard, m33td_sdcard
##CASE_stm32mp2-m33td##    "tfm_dt_name" is the devicetree use with TF-M compilation
##CASE_stm32mp2-m33td##    "your_cube_m33td_project_name" is the name of project used on M33 processor
##CASE_stm32mp2-m33td##    "your_cube_m33td_project_path" is the path for project used on M33 processor
##CASE_stm32mp2-m33td##    For runtime binaries
##CASE_stm32mp2-m33td##    $@C> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/m33-projects M33FW_CONFIG=<your_storage_boot_scheme_cortex_m> M33FW_BOARDS=<your_board_name> M33FW_PROJECT_NAME=<your_cube_m33td_project_name> M33FW_PROJECT_PATH=<your_cube_m33td_project_path> M33FW_DEVICETREE=<tfm_dt_name> M33FW_EXTRA_OPTFLAGS=<your_cube_m33td_bld_opts> m33td
##CASE_stm32mp2-m33td##    For M33FW binaries
##CASE_stm32mp2-m33td##    $@M> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/m33-projects M33FW_CONFIG=<your_storage_boot_scheme_cortex_m> M33FW_BOARDS=<your_board_name> M33FW_PROJECT_NAME=<your_cube_m33td_project_name> M33FW_PROJECT_PATH=<your_cube_m33td_project_path> M33FW_DEVICETREE=<tfm_dt_name> M33FW_EXTRA_OPTFLAGS=<your_cube_m33td_bld_opts> m33fw
##CASE_stm32mp2-m33td##    Example m33td_sdcard binaries
##CASE_stm32mp2-m33td##      Example for runtime binaries
##CASE_stm32mp2-m33td##        $MP21> make -f ../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/m33-projects M33FW_CONFIG=m33td_sdcard M33FW_DEVICETREE=stm32mp215f-dk-cm33tdcid-ostl  M33FW_BOARDS=stm32mp215f-dk  M33FW_PROJECT_NAME=StarterApp_M33TD M33FW_PROJECT_PATH=Projects/STM32MP215F-DK/Demonstrations/StarterApp_M33TD m33td
##CASE_stm32mp2-m33td##        $MP25> make -f ../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/m33-projects M33FW_CONFIG=m33td_sdcard M33FW_DEVICETREE=stm32mp257f-ev1-cm33tdcid-ostl M33FW_BOARDS=stm32mp257f-ev1 M33FW_PROJECT_NAME=StarterApp_M33TD M33FW_PROJECT_PATH=Projects/STM32MP257F-EV1/Demonstrations/StarterApp_M33TD m33td
##CASE_stm32mp2-m33td##      Example for M33FW binaries
##CASE_stm32mp2-m33td##        $MP21> make -f ../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/m33-projects M33FW_CONFIG=m33td_sdcard M33FW_DEVICETREE=stm32mp215f-dk-cm33tdcid-ostl  M33FW_BOARDS=stm32mp215f-dk  M33FW_PROJECT_NAME=StarterApp_M33TD M33FW_PROJECT_PATH=Projects/STM32MP215F-DK/Demonstrations/StarterApp_M33TD m33fw
##CASE_stm32mp2-m33td##        $MP25> make -f ../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/m33-projects M33FW_CONFIG=m33td_sdcard M33FW_DEVICETREE=stm32mp257f-ev1-cm33tdcid-ostl M33FW_BOARDS=stm32mp257f-ev1 M33FW_PROJECT_NAME=StarterApp_M33TD M33FW_PROJECT_PATH=Projects/STM32MP257F-EV1/Demonstrations/StarterApp_M33TD m33fw
##CASE_stm32mp2-m33td##    Example m33td_emmc binaries
##CASE_stm32mp2-m33td##      Example for runtime binaries
##CASE_stm32mp2-m33td##        $MP25> make -f ../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/m33-projects M33FW_CONFIG=m33td_emmc M33FW_DEVICETREE=stm32mp257f-ev1-cm33tdcid-ostl M33FW_BOARDS=stm32mp257f-ev1 M33FW_PROJECT_NAME=StarterApp_M33TD M33FW_PROJECT_PATH=Projects/STM32MP257F-EV1/Demonstrations/StarterApp_M33TD m33td
##CASE_stm32mp2-m33td##      Example for M33FW binaries
##CASE_stm32mp2-m33td##        $MP25> make -f ../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/m33-projects M33FW_CONFIG=m33td_emmc M33FW_DEVICETREE=stm32mp257f-ev1-cm33tdcid-ostl M33FW_BOARDS=stm32mp257f-ev1 M33FW_PROJECT_NAME=StarterApp_M33TD M33FW_PROJECT_PATH=Projects/STM32MP257F-EV1/Demonstrations/StarterApp_M33TD m33fw
##CASE_stm32mp2-m33td##    Example m33td_nor-sdcard binaries
##CASE_stm32mp2-m33td##      Example for runtime binaries
##CASE_stm32mp2-m33td##        $MP25> make -f ../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/m33-projects M33FW_CONFIG=m33td_nor-sdcard M33FW_DEVICETREE=stm32mp257f-ev1-cm33tdcid-ostl M33FW_BOARDS=stm32mp257f-ev1 M33FW_PROJECT_NAME=StarterApp_M33TD M33FW_PROJECT_PATH=Projects/STM32MP257F-EV1/Demonstrations/StarterApp_M33TD m33td
##CASE_stm32mp2-m33td##      Example for M33FW binaries
##CASE_stm32mp2-m33td##        $MP25> make -f ../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/m33-projects M33FW_CONFIG=m33td_nor-sdcard M33FW_DEVICETREE=stm32mp257f-ev1-cm33tdcid-ostl M33FW_BOARDS=stm32mp257f-ev1 M33FW_PROJECT_NAME=StarterApp_M33TD M33FW_PROJECT_PATH=Projects/STM32MP257F-EV1/Demonstrations/StarterApp_M33TD m33fw
##CASE_stm32mp2-m33td##    Example m33td_nor-emmc binaries
##CASE_stm32mp2-m33td##      Example for runtime binaries
##CASE_stm32mp2-m33td##        $MP25> make -f ../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/m33-projects M33FW_CONFIG=m33td_nor-emmc M33FW_DEVICETREE=stm32mp257f-ev1-cm33tdcid-ostl M33FW_BOARDS=stm32mp257f-ev1 M33FW_PROJECT_NAME=StarterApp_M33TD M33FW_PROJECT_PATH=Projects/STM32MP257F-EV1/Demonstrations/StarterApp_M33TD m33td
##CASE_stm32mp2-m33td##      Example for M33FW binaries
##CASE_stm32mp2-m33td##        $MP25> make -f ../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/m33-projects M33FW_CONFIG=m33td_nor-emmc M33FW_DEVICETREE=stm32mp257f-ev1-cm33tdcid-ostl M33FW_BOARDS=stm32mp257f-ev1 M33FW_PROJECT_NAME=StarterApp_M33TD M33FW_PROJECT_PATH=Projects/STM32MP257F-EV1/Demonstrations/StarterApp_M33TD m33fw
##CASE_stm32mp2-m33td##    Example m33td_nor binaries
##CASE_stm32mp2-m33td##      Example for runtime binaries
##CASE_stm32mp2-m33td##        $MP25> make -f ../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/m33-projects M33FW_CONFIG=m33td_nor M33FW_DEVICETREE=stm32mp257f-ev1-cm33tdcid-ostl M33FW_BOARDS=stm32mp257f-ev1 M33FW_PROJECT_NAME=StarterApp_M33TD M33FW_PROJECT_PATH=Projects/STM32MP257F-EV1/Demonstrations/StarterApp_M33TD m33td
##CASE_stm32mp2-m33td##      Example for M33FW binaries
##CASE_stm32mp2-m33td##        $MP25> make -f ../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$M33FW_DEPLOYDIR_ROOT/m33-projects M33FW_CONFIG=m33td_nor M33FW_DEVICETREE=stm32mp257f-ev1-cm33tdcid-ostl M33FW_BOARDS=stm32mp257f-ev1 M33FW_PROJECT_NAME=StarterApp_M33TD M33FW_PROJECT_PATH=Projects/STM32MP257F-EV1/Demonstrations/StarterApp_M33TD m33fw
