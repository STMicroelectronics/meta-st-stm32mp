Compilation of Optee-os (Trusted Execution Environment):
1. Pre-requisite
2. Initialize cross-compilation via SDK
3. Prepare optee-os source code
4. Manage optee-os source code
5. Compile optee-os source code
6. Update software on board
7. Update starter package with optee-os compilation outputs

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
    $ source <path to SDK>/environment-setup-cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi

 To verify if your cross-compilation environment have put in place:
    $ set | grep CROSS
    CROSS_COMPILE=arm-ostl-linux-gnueabi-

Warning: the environment are valid only on the shell session where you have
 sourced the sdk environment.

--------------------------
3. Prepare optee-os source
--------------------------
If not already done, extract the sources from Developer Package tarball, for example:
    $ tar xf en.SOURCES-stm32mp1-*.tar.xz

In the optee-os source directory (sources/*/##BP##-##PR##),
you have one optee-os source tarball, the patches and one Makefile:
   - ##BP##-##PR##.tar.xz
   - 00*.patch
   - Makefile.sdk

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
Since OpenSTLinux activates FIP by default, FIP_artifacts directory path must be specified before launching compilation
  - In case of using SOURCES-xxxx.tar.gz of Developer package the FIP_DEPLOYDIR_ROOT must be set as below:
    $> export FIP_DEPLOYDIR_ROOT=$PWD/../../FIP_artifacts

The build results for this component are available in DEPLOYDIR (Default: $PWD/../deploy).
If needed, this deploy directory can be specified by adding "DEPLOYDIR=<your_deploy_dir_path>" compilation option to the build command line below.
The generated FIP images are available in $FIP_DEPLOYDIR_ROOT/fip

To list optee-os source code compilation configurations:
    $ make -f $PWD/../Makefile.sdk help
To compile optee-os source code:
    $ make -f $PWD/../Makefile.sdk all
To compile optee-os source code for a specific config:
    $ make -f $PWD/../Makefile.sdk CFG_EMBED_DTB_SOURCE_FILE=stm32mp157c-ev1 all
To compile optee-os source code and overwrite the default FIP artifacts with built artifacts:
    $> make -f $PWD/../Makefile.sdk DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee all

---------------------------
6. Update software on board
---------------------------
Please use STM32CubeProgrammer and only tick the ssbl-boot and fip partitions (more informations on the wiki website http://wiki.st.com/stm32mpu)

---------------------------
7. Update Starter Package with optee-os compilation outputs
---------------------------
If not already done, extract the artifacts from Starter Package tarball, for example:
    # tar xf en.FLASH-stm32mp1-*.tar.xz

Move to Starter Package root folder,
    #> cd <your_starter_package_dir_path>
Cleanup Starter Package from original optee-os artifacts first
    #> rm -rf images/stm32mp1/fip/*
Update Starter Package with new fip artifacts from <FIP_DEPLOYDIR_ROOT>/fip folder:
    #> cp -rvf $FIP_DEPLOYDIR_ROOT/fip/* images/stm32mp1/fip/

Then the new Starter Package is ready to use for "Image flashing" on board (more information on wiki website https://wiki.st.com/stm32mpu).
