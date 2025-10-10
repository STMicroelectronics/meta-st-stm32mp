def clean_tarball(d, ar_workdir='', ar_outdir=''):
    from os import listdir
    import tarfile
    import shutil, tempfile
    import os
    import re

    if not ar_workdir:
        ar_workdir = d.getVar('ARCHIVER_WORKDIR')
    if not ar_outdir:
        ar_outdir = d.getVar('ARCHIVER_OUTDIR')
    if not os.path.exists(ar_outdir):
        return

    #get tarball name
    compression_method = d.getVarFlag('ARCHIVER_MODE', 'compression')
    tarball_name_list = [f for f in listdir(ar_outdir) if f.endswith("tar.%s" % compression_method)]
    if not tarball_name_list:
        return

    tmpdir = tempfile.mkdtemp(dir=ar_workdir)
    for tarball_name in tarball_name_list:
        tar = tarfile.open(os.path.join(ar_outdir,tarball_name))
        tar.extractall(path=tmpdir)
        tar.close()
        dirs_list = [f for f in listdir(tmpdir) if os.path.isdir(os.path.join(tmpdir, f))]
        if len(dirs_list) == 1:
            if dirs_list[0] == "git":
                subdir = ''
            else:
                subdir = dirs_list[0]
            temp_extract_subdir = os.path.join(tmpdir,subdir)

            if d.getVarFlag('ARCHIVER_MODE', 'src') == "original" and tarball_name != d.getVar('PF') + '.tar.' + compression_method:
                suffix = re.sub(r'%s-(.*)\.tar\.%s' % (d.getVar('PF'),compression_method), r'\1', tarball_name)
            else:
                suffix = ''

            targeted_dir_source = os.path.join(temp_extract_subdir,d.getVar('BPN')+'-'+d.getVar('PV'),suffix)
            uncompressed_source = os.path.join(temp_extract_subdir,'git')

            if os.path.exists(os.path.join(uncompressed_source,'.git')):
                shutil.rmtree(os.path.join(uncompressed_source,'.git'))
                # Remove all '.git' folders from tree
                for root, dirs, files in os.walk(uncompressed_source):
                    if '.git' in dirs:
                        shutil.rmtree(os.path.join(root,'.git'))
                # Feed target dir with clean folder
                shutil.move(uncompressed_source,targeted_dir_source)
                # Init source path for tarball
                subdirs_list = [d for d in listdir(temp_extract_subdir) if os.path.isdir(os.path.join(temp_extract_subdir,d))]
                if len(subdirs_list) == 1:
                    src_origin = os.path.join(temp_extract_subdir,subdirs_list[0])
                else:
                    src_origin = os.path.join(temp_extract_subdir,'.')
                # Cleanup tarball file before creation
                os.remove(os.path.join(ar_outdir,tarball_name))
                create_tarball(d, src_origin, suffix, ar_outdir)
        # Clean temporary dir
        bb.utils.remove(tmpdir, recurse=True)

python archiver_clean_tarball() {
    clean_tarball(d)
}
do_ar_original[postfuncs] =+ "archiver_clean_tarball"

# We also expect cleanup on patched tarball(s), so add call to clean on do_ar_patched
do_ar_patched:append() {
    clean_tarball(d, ar_workdir, ar_outdir)
}

# We also expect cleanup on configured tarball(s), so add call to clean on do_ar_configured
do_ar_configured:append() {
    clean_tarball(d, d.getVar('WORKDIR'), ar_outdir)
}

ARCHIVER_README = "README.HOW_TO.txt"

do_archiver_git_uri() {
    if [ "$(echo "${SRC_URI}" | grep branch | wc -l)" -gt 0 ]; then
        BRANCH=$(echo "${SRC_URI}" | sed "s|.*branch=\([^ ;]*\).*|\1|")
    else
        BRANCH=main
    fi
    if [ -z "${ARCHIVER_ST_BRANCH}" ]; then
        ARCHIVER_ST_BRANCH="${BRANCH}"
    fi
    if [ -z "${ARCHIVER_ST_REVISION}" ]; then
        ARCHIVER_ST_REVISION="${SRCREV}"
    fi
    if [ -e "${ARCHIVER_OUTDIR}/${ARCHIVER_README}.${MACHINE}" ]; then
        bbnote "Use ${ARCHIVER_README}.${MACHINE} file for update from ${ARCHIVER_OUTDIR}"
    elif [ -e "${ARCHIVER_OUTDIR}/${ARCHIVER_README}" ]; then
        bbnote "Use ${ARCHIVER_README} file for update from ${ARCHIVER_OUTDIR} (update to ${ARCHIVER_README}.${MACHINE})"
        install -m 644 "${ARCHIVER_OUTDIR}/${ARCHIVER_README}" "${ARCHIVER_OUTDIR}/${ARCHIVER_README}.${MACHINE}"
        rm "${ARCHIVER_OUTDIR}/${ARCHIVER_README}"
    elif [ -e "${ARCHIVER_WORKDIR}/${ARCHIVER_README}" ]; then
        bbnote "Use ${ARCHIVER_README} file for update from ${ARCHIVER_WORKDIR} (update to ${ARCHIVER_README}.${MACHINE})"
        install -d "${ARCHIVER_OUTDIR}"
        install -m 644 "${ARCHIVER_WORKDIR}/${ARCHIVER_README}" "${ARCHIVER_OUTDIR}/${ARCHIVER_README}.${MACHINE}"
    else
        bbnote "No ${ARCHIVER_README} file found for update."
        return
    fi

    sed -i -e "s|##LINUX_TARNAME##|${LINUX_TARNAME}|g" \
           -e "s|##LINUX_TARBASE##|${LINUX_TARBASE}|g" \
           -e "s|##GCNANO_TARNAME##|${GCNANO_TARNAME}|g" \
           -e "s|##ARCHIVER_COMMUNITY_BRANCH##|${ARCHIVER_COMMUNITY_BRANCH}|g" \
           -e "s|##ARCHIVER_COMMUNITY_REVISION##|${ARCHIVER_COMMUNITY_REVISION}|g" \
           -e "s|##ARCHIVER_ST_BRANCH##|${ARCHIVER_ST_BRANCH}|g" \
           -e "s|##ARCHIVER_ST_REVISION##|${ARCHIVER_ST_REVISION}|g" \
           -e "s|##BP##|${BP}|g" \
           -e "s|##PV##|${PV}|g" \
           -e "s|##PR##|${PR}|g" \
           -e "s|##MACHINE##|${MACHINE}|g" \
           -e "s|##KERNEL_VERSION##|${KERNEL_VERSION}|g" \
           "${ARCHIVER_OUTDIR}/${ARCHIVER_README}.${MACHINE}"

    sed -i -e "s|##CASE_${MACHINE}##||g" "${ARCHIVER_OUTDIR}/${ARCHIVER_README}.${MACHINE}"
    sed -i -e "/^##CASE_/d" "${ARCHIVER_OUTDIR}/${ARCHIVER_README}.${MACHINE}"
}
do_archiver_git_uri[dirs] = "${ARCHIVER_WORKDIR} ${ARCHIVER_OUTDIR}"
addtask do_archiver_git_uri before do_deploy_archives after do_ar_original do_unpack_and_patch
