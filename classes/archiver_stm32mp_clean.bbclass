python archiver_clean_tarball() {
    from os import listdir
    import tarfile
    import shutil, tempfile
    import os

    ar_outdir = d.getVar('ARCHIVER_OUTDIR')
    compression_method = d.getVarFlag('ARCHIVER_MODE', 'compression')
    #get tarball name
    tarball_name = [f for f in listdir(ar_outdir) if f.endswith("tar.%s" % compression_method)]
    tmpdir = tempfile.mkdtemp(dir=d.getVar('ARCHIVER_WORKDIR'))
    if tarball_name and tarball_name[0] and len(tarball_name[0]) > 0:
        tar = tarfile.open(os.path.join(ar_outdir,tarball_name[0]))
        tar.extractall(path=tmpdir)
        tar.close()
        dirs_list = [f for f in listdir(tmpdir) if os.path.isdir(os.path.join(tmpdir, f))]
        if len(dirs_list) == 1:
            if os.path.exists(os.path.join(tmpdir,dirs_list[0],"git", ".git")):
                src_origin = os.path.join(tmpdir,dirs_list[0], '.')
                shutil.rmtree(os.path.join(tmpdir,dirs_list[0],"git", ".git"))
                shutil.move(os.path.join(tmpdir,dirs_list[0],"git"),os.path.join(tmpdir,dirs_list[0],d.getVar('BPN')+'-'+d.getVar('PV')))
                os.remove(os.path.join(ar_outdir,tarball_name[0]))
                subdirs_list = [f for f in listdir(os.path.join(tmpdir,dirs_list[0])) if os.path.isdir(os.path.join(tmpdir,dirs_list[0], f))]
                if len(subdirs_list) == 1:
                    src_origin = os.path.join(tmpdir,dirs_list[0],subdirs_list[0])
                create_tarball(d, src_origin, '', ar_outdir)
}
do_ar_original[postfuncs] =+ "archiver_clean_tarball"

ARCHIVER_README = "README.HOW_TO.txt"

archiver_git_uri() {
    ret=$(echo "${SRC_URI}" | grep branch | wc -l)
    if [ $ret -gt 0 ]; then
        BRANCH=`echo "${SRC_URI}" | sed "s|.*branch=\([^ ;]*\).*|\1|" `
    else
        BRANCH=master
    fi

    if [ -z "${ARCHIVER_ST_BRANCH}" ]; then
        ARCHIVER_ST_BRANCH="${BRANCH}"
    fi
    if [ -z "${ARCHIVER_ST_REVISION}" ]; then
        ARCHIVER_ST_REVISION="${SRCREV}"
    fi

    if [ -e "${ARCHIVER_OUTDIR}/${ARCHIVER_README}" ]; then
        sed -i -e "s|##LINUX_TARNAME##|${LINUX_TARNAME}|g" -e "s|##GCNANO_TARNAME##|${GCNANO_TARNAME}|g" -e "s|##ARCHIVER_COMMUNITY_BRANCH##|${ARCHIVER_COMMUNITY_BRANCH}|g" -e "s|##ARCHIVER_COMMUNITY_REVISION##|${ARCHIVER_COMMUNITY_REVISION}|g" -e "s|##ARCHIVER_ST_BRANCH##|${ARCHIVER_ST_BRANCH}|g" -e "s|##ARCHIVER_ST_REVISION##|${ARCHIVER_ST_REVISION}|g" -e "s|##BP##|${BP}|g" -e "s|##PV##|${PV}|g" -e "s|##PR##|${PR}|g" "${ARCHIVER_OUTDIR}/${ARCHIVER_README}"
    fi
}
do_ar_original[postfuncs] =+ "archiver_git_uri"

