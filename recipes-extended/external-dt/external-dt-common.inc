FILESEXTRAPATHS:prepend := "${THISDIR}/external-dt:"

PACKAGE_ARCH = "${MACHINE_ARCH}"

inherit external-dt

# Make sure to move ${S} to STAGING_EXTDT_DIR. We can't just
# create the symlink in advance as the git fetcher can't cope with
# the symlink.
do_unpack[cleandirs] += "${S}"
do_unpack[cleandirs] += "${@bb.utils.contains('EXTERNAL_DT_ENABLED', '1', '${STAGING_EXTDT_DIR}', '', d)}"
do_clean[cleandirs] += "${S}"
do_clean[cleandirs] += "${@bb.utils.contains('EXTERNAL_DT_ENABLED', '1', '${STAGING_EXTDT_DIR}', '', d)}"

python do_symlink_externaldtsrc() {
    # Specific part to update devtool-source class
    if bb.data.inherits_class('devtool-source', d):
        # We don't want to move the source to STAGING_EXTDT_DIR here
        if d.getVar('STAGING_EXTDT_DIR', d):
            d.setVar('STAGING_EXTDT_DIR', '${S}')

    shared = d.getVar("EXTERNAL_DT_ENABLED")
    if shared and oe.types.boolean(shared):
        # Copy/Paste from kernel class with adaptation to EXT DT var
        s = d.getVar("S")
        if s[-1] == '/':
            # drop trailing slash, so that os.symlink(extdtsrc, s) doesn't use s as directory name and fail
            s=s[:-1]
        extdtsrc = d.getVar("STAGING_EXTDT_DIR")
        if s != extdtsrc:
            bb.utils.mkdirhier(extdtsrc)
            bb.utils.remove(extdtsrc, recurse=True)
            if d.getVar("EXTERNALSRC"):
                # With EXTERNALSRC S will not be wiped so we can symlink to it
                os.symlink(s, extdtsrc)
            else:
                import shutil
                shutil.move(s, extdtsrc)
                os.symlink(extdtsrc, s)
}
addtask symlink_externaldtsrc before do_patch do_configure after do_unpack
