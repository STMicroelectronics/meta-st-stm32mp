Sharing external device tree
1. Prepare external device tree source
2. Manage external dt source code with GIT
3. Configure external device tree source code
4. Test external device tree source code

--------------------------------------
1. Prepare external device tree source
--------------------------------------
If not already done, extract the sources from Developer Package tarball, for example:
    $ tar xf en.SOURCES-stm32mp2-*.tar.xz

In the external device tree source directory (sources/*/##BP##-##PR##),
you have one external dt tarball:
   - ##BP##-##PR##.tar.xz
 
If you would like to have a git management for the source code move to
to section 2 [Management of external dt source code with GIT].

Otherwise, to manage external dt source code without git, you must extract the
tarball now and apply the patch:

    $> tar xf ##BP##-##PR##.tar.xz

You can now move to section 3 [Configure external device tree source code].

-------------------------------------
2. Manage external dt source code with GIT
-------------------------------------
If you like to have a better management of change made on external dt source, you
have following solutions to use git.

2.1 Create Git from tarball
---------------------------
    $ cd <directory to external dt source code>
    $ test -d .git || git init . && git add . && git commit -m "new dt" && git gc
    $ git checkout -b WORKING
  NB: this is the fastest way to get your external dt source code ready for development

-------------------------------
3. Configure external device tree source code
-------------------------------
To enable use of external device tree source code for other component, you must
set the EXTDT_DIR variable to your shell environement:

    $> export EXTDT_DIR=$PWD/##BP##

---------------------------
4. Test external device tree source code
---------------------------
Nothing to do, external device tree is directly used by other component.

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
    $@S> export EXTDT_DIR=$PWD/##BP##

