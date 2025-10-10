Sharing tf-m-tests-stm32mp source
1. Prepare tf-m-tests-stm32mp source
2. Manage tf-m-tests-stm32mp source code with GIT
3. Configure tf-m-tests-stm32mp source code
4. Test tf-m-tests-stm32mp source code

--------------------------------------
1. Prepare tf-m-tests-stm32mp source
--------------------------------------
If not already done, extract the sources from Developer Package tarball, for example:
    $ tar xf en.SOURCES-stm32mp2-*.tar.xz

In the tf-m-tests-stm32mp source directory (sources/*/##BP##-##PR##),
you have one tf-m-tests-stm32mp tarball:
   - ##BP##-##PR##.tar.xz
 
If you would like to have a git management for the source code move to
to section 2 [Management of tf-m-tests-stm32mp source code with GIT].

Otherwise, to manage tf-m-tests-stm32mp source code without git, you must extract the
tarball now and apply the patch:

    $> tar xf ##BP##-##PR##.tar.xz
    $> cd ##BP##
    $> for p in `ls -1 ../*.patch`; do patch -p1 < $p; done
    $> cd ..

You can now move to section 3 [Configure tf-m-tests-stm32mp source code].

-------------------------------------
2. Manage tf-m-tests-stm32mp source code with GIT
-------------------------------------
If you like to have a better management of change made on tf-m-tests-stm32mp source, you
have following solutions to use git.

2.1 Get STMicroelectronics tf-m-tests-stm32mp source from GitHub
--------------------------------------------------
    URL: https://github.com/STMicroelectronics/tf-m-tests.git
    Branch: ##ARCHIVER_ST_BRANCH##
    Revision: ##ARCHIVER_ST_REVISION##

    $ git clone https://github.com/STMicroelectronics/tf-m-tests.git
    $ git checkout -b WORKING ##ARCHIVER_ST_REVISION##

2.2 Create Git from tarball
---------------------------
    $ tar xf ##BP##-##PR##.tar.xz
    $ cd ##BP##
    $ test -d .git || git init . && git add . && git commit -m "tf-m-tests source code" && git gc
    $ git checkout -b WORKING
    $ for p in `ls -1 ../*.patch`; do git am $p; done
    $ cd ..

2.3 Get Git from Arm Software community and apply STMicroelectronics patches
---------------------------------------------------------------
    URL: git://git.trustedfirmware.org/TF-M/tf-m-tests.git
    Branch: ##ARCHIVER_COMMUNITY_BRANCH##
    Revision: ##ARCHIVER_COMMUNITY_REVISION##

    $ git clone git://git.trustedfirmware.org/TF-M/tf-m-tests.git
    $ cd tf-m-tests
    $ git checkout -b WORKING ##ARCHIVER_COMMUNITY_REVISION##
    $ for p in `ls -1 <path to patch>/*.patch`; do git am $p; done
    $ cd ..

-------------------------------
3. Configure tf-m-tests-stm32mp source code
-------------------------------
To enable use of tf-m-tests-stm32mp source code for other component, you must
set the TFM_TESTS_DIR variable to your shell environement:

    $> export TFM_TESTS_DIR=$PWD/##BP##

---------------------------
4. Test tf-m-tests-stm32mp source code
---------------------------
Nothing to do, tf-m-tests-stm32mp is directly used by other component.

    #> echo "*** Nothing to test ***"

----------------------------
5. Example of compilation usage
----------------------------
    $@E> cd ##BP##-##PR##
    $@E> tar xf ##BP##-##PR##.tar.xz
    $@E> cd ##BP##
    $@E> for p in `ls -1 ../*.patch`; do patch -p1 < $p; done
    $    cd ..
    $    cd ..
    $@P> cd ##BP##-##PR##
    $@P> cd ##BP##
    $@S> cd ..
    $@S> export TFM_TESTS_DIR=$PWD/##BP##

