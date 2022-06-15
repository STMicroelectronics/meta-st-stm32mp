Compilation of Optee-os (Trusted Execution Environment):
1. Pre-requisite
2. Initialize cross-compilation via SDK
3. Prepare optee-os source code
4. Manage optee-os source code
5. Compile optee-os source code
6. Update software on board

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
$ tar xfJ en.SOURCES-stm32mp1-*.tar.xz

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
    $> tar xfz ../fonts.tar.gz
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
    $ cd <optee-os source>
    $ test -d .git || git init . && git add . && git commit -m "optee-os source code" && git gc
    $ git checkout -b WORKING
    $ tar xfz ../fonts.tar.gz
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
    $ tar xfz <path to patch>/fonts.tar.gz
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

To compile optee-os source code with default embedded config:
    $> make -f $PWD/../Makefile.sdk all
You can check the configuration throught 'help' target:
    $ make -f $PWD/../Makefile.sdk help

To compile only one of the provided devicetree:
    $ make -f $PWD/../Makefile.sdk CFG_EMBED_DTB_SOURCE_FILE=stm32mp157f-ev1 all
For a specific devicetree file you need to force not only the devicetree but also the DRAM size settings:
    $ make -f $PWD/../Makefile.sdk CFG_EMBED_DTB_SOURCE_FILE=<your_devicetree> OPTEE_DRAMSIZE=<dram_size_value_in_hexadecimal> all

By default, the build results for this component are available in $PWD/../deploy directory.
If needed, this deploy directory can be specified by added "DEPLOYDIR=<your_deploy_dir_path>" compilation option to the build command line above.
In case DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/optee it overwrites files directly in FIP artifacts directory.

The generated FIP images are available in $FIP_DEPLOYDIR_ROOT/fip

---------------------------
6. Update software on board
---------------------------
Please use STM32CubeProgrammer and only tick the ssbl-boot and fip partitions (more informations on the wiki website http://wiki.st.com/stm32mpu)

