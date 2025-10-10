FILESEXTRAPATHS:prepend := "${THISDIR}/tf-m-tests-stm32mp:"

PACKAGE_ARCH = "${MACHINE_ARCH}"

# Init far STAGING_TF_M_TESTS_DIR
require tf-m-tests-stm32mp-share.inc

# Do not remove source code, even if rm_work is configured
RM_WORK_EXCLUDE += "${PN}"

# -----------------------------------------------
# Enable use of work-shared folder
# Make sure to move ${S} to STAGING_TF_M_TESTS_DIR. We can't just
# create the symlink in advance as the git fetcher can't cope with
# the symlink.
do_unpack[cleandirs] += "${S}"
do_unpack[cleandirs] += "${STAGING_TF_M_TESTS_DIR}"
do_clean[cleandirs] += "${S}"
do_clean[cleandirs] += "${STAGING_TF_M_TESTS_DIR}"
python do_symlink_src() {
    # Specific part to update devtool-source class
    if bb.data.inherits_class('devtool-source', d):
        # We don't want to move the source to STAGING_TF_M_TESTS_DIR here
        if d.getVar('STAGING_TF_M_TESTS_DIR', d):
            d.setVar('STAGING_TF_M_TESTS_DIR', '${S}')

    # Copy/Paste from kernel class with adaptation to SCPFW var
    s = d.getVar("S")
    if s[-1] == '/':
        # drop trailing slash, so that os.symlink(src, s) doesn't use s as directory name and fail
        s=s[:-1]
    src = d.getVar("STAGING_TF_M_TESTS_DIR")
    if s != src:
        bb.utils.mkdirhier(src)
        bb.utils.remove(src, recurse=True)
        if d.getVar("EXTERNALSRC"):
            # With EXTERNALSRC S will not be wiped so we can symlink to it
            os.symlink(s, src)
        else:
            import shutil
            shutil.move(s, src)
            os.symlink(src, s)
}
addtask symlink_src before do_patch do_configure after do_unpack
