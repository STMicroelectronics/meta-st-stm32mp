Sharing firmware for stm32mp-ddr-phy
1. Prepare stm32mp-ddr-phy source
2. Manage stm32mp-ddr-phy source code with GIT
3. Configure stm32mp-ddr-phy source code
4. Test stm32mp-ddr-phy source code

--------------------------------------
1. Prepare stm32mp-ddr-phy source
--------------------------------------
If not already done, extract the sources from Developer Package tarball, for example:
    $ tar xf en.SOURCES-stm32mp*-*.tar.xz

In the stm32mp-ddr-phy source directory (sources/*/##BP##-##PR##),
you have one stm32mp-ddr-phy tarball:
   - ##BP##-##PR##.tar.xz

If you would like to have a git management for the source code move to
to section 2 [Management of stm32mp-ddr-phy source code with GIT].

Otherwise, to manage stm32mp-ddr-phy source code without git, you must extract the
tarball now and apply the patch:

    $> tar xf ##BP##-##PR##.tar.xz

You can now move to section 3 [Configure stm32mp-ddr-phy source code].

-------------------------------------
2. Manage stm32mp-ddr-phy source code with GIT
-------------------------------------
If you like to have a better management of change made on stm32mp-ddr-phy source, you
have following solutions to use git.

2.1 Create Git from tarball
---------------------------
    $ cd <directory to stm32mp-ddr-phy source code>
    $ test -d .git || git init . && git add . && git commit -m "new stm32mp-ddr-phy" && git gc
    $ git checkout -b WORKING
  NB: this is the fastest way to get your stm32mp-ddr-phy source code ready for development

-------------------------------
3. Configure stm32mp-ddr-phy source code
-------------------------------
To enable use of stm32mp-ddr-phy source code for other component, you must set the
FWDDR_DIR variable to your shell environement:

    $> export FWDDR_DIR=$PWD/##BP##

---------------------------
4. Test stm32mp-ddr-phy source code
---------------------------
Nothing to do, stm32mp-ddr-phy is directly used by other component.

    #> echo "*** Nothing to test ***"

----------------------------
5. Example of compilation usage
----------------------------
    $@E> cd ##BP##-##PR##
    $@E> tar xf ##BP##-##PR##.tar.xz
    $@E> cd ##BP##

    $ cd ..
    $ cd ..
    $@P> cd ##BP##-##PR##
    $@P> cd ##BP##
    $@S> cd ..
    $@S> export FWDDR_DIR=$PWD/##BP##
