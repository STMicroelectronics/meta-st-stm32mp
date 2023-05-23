SRC_URI:append = " file://README.HOW_TO.txt "

inherit archiver
ARCHIVER_MODE[src] = "original"

# Source code directory relative path inside archive
AR_ORIGINAL_SET_SUBDIR ?= "${BPN}"

# Keep only gcnano-driver source code in generated tarball
# Original archiver driver tarball:
# + gcnano-driver-stm32mp-${GCNANO_VERSION}-${PR}/
#   + gcnano-driver-stm32mp/
#     + <driver source code>
#   + gcnano-userland-*.(bin|tar.xz)
# Final archiver driver tarball:
# + gcnano-driver-stm32mp-${GCNANO_VERSION}-${PR}/
#   + <driver source code>
python do_replace_archiver_tarball() {
    import tarfile, tempfile, shutil

    archiver_outdir = d.getVar('ARCHIVER_OUTDIR')
    archiver_workdir = d.getVar('ARCHIVER_WORKDIR')
    bp = d.getVar('BP')
    compression_method = d.getVarFlag('ARCHIVER_MODE', 'compression')
    pf = d.getVar('PF')
    ar_subdir = d.getVar('AR_ORIGINAL_SET_SUBDIR')

    # Extract the original tarball in a temporary directory
    tarball_filename = '%s.tar.%s' % (pf, compression_method)
    tarball_filepath = os.path.join(archiver_outdir, tarball_filename)

    if not os.path.isfile(tarball_filepath):
        bb.fatal('Original tarball does not exist: %s' % tarball_filepath)

    tmpdir = tempfile.mkdtemp(dir=archiver_workdir)
    with tarfile.open(tarball_filepath, 'r:%s' % compression_method) as tarball:
        tarball.extractall(path=tmpdir)

    # Get the selected subdirectory and use it to replace original archive root directory
    root_dirpath = os.path.join(tmpdir, bp)
    src_dirpath = os.path.join(root_dirpath, ar_subdir)

    if not os.path.isdir(root_dirpath):
        bb.fatal('Archive root directory not found: %s' % root_dirpath)

    if not os.path.isdir(src_dirpath):
        bb.fatal('Subdirectory not found: %s' % src_dirpath)

    bb.note('Clean archive content and keep only subdirectory %s' % ar_subdir)

    shutil.move(src_dirpath, tmpdir)
    shutil.rmtree(root_dirpath)

    src_dirpath = os.path.join(tmpdir, ar_subdir)
    shutil.move(src_dirpath, root_dirpath)

    # Archive new directory and replace the original tarball
    tmp_tarball_filepath = os.path.join(tmpdir, tarball_filename)
    with tarfile.open(tmp_tarball_filepath, 'w:%s' % compression_method) as tarball:
        tarball.add(root_dirpath, arcname=os.path.basename(root_dirpath))
    shutil.copy(tmp_tarball_filepath, tarball_filepath)
}
do_ar_original[postfuncs] =+ "do_replace_archiver_tarball"

inherit archiver_stm32mp_clean
