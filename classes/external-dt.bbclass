inherit externalsrc

EXTERNAL_DT_ENABLED ??= "1"

EXTDT_SRC_PROVIDER ??= "external-dt"

STAGING_EXTDT_DIR ??= "${TMPDIR}/work-shared/${MACHINE}/${EXTDT_SRC_PROVIDER}"

EXTDT_USE_SUFFIX ??= "0"
# Init default suffix per storage
EXTDT_SUFFIX_STORAGE ??= "EMMC NAND NOR SDCARD SPINAND"
EXTDT_SUFFIX_EMMC    ??= ""
EXTDT_SUFFIX_NAND    ??= ""
EXTDT_SUFFIX_NOR     ??= ""
EXTDT_SUFFIX_SDCARD  ??= ""
EXTDT_SUFFIX_SPINAND ??= ""

EXTDT_ROOTDIR ??= ""

EXTDT_DIR_TF_A  ??= "${EXTDT_ROOTDIR}tf-a"
EXTDT_DIR_TF_A_SERIAL  ??= "${EXTDT_ROOTDIR}tf-a"
EXTDT_DIR_UBOOT ??= "${EXTDT_ROOTDIR}u-boot"
EXTDT_DIR_UBOOT_SERIAL ??= "${EXTDT_ROOTDIR}u-boot"
EXTDT_DIR_MCU   ??= "${EXTDT_ROOTDIR}mcuboot"
EXTDT_DIR_TF_M  ??= "${EXTDT_ROOTDIR}tfm"
EXTDT_DIR_OPTEE ??= "${EXTDT_ROOTDIR}optee"
EXTDT_DIR_OPTEE_SERIAL ??= "${EXTDT_ROOTDIR}optee"
EXTDT_DIR_LINUX ??= "${EXTDT_ROOTDIR}linux"

EXTDT_DIR_CONFIG += "virtual/trusted-firmware-a:${EXTDT_DIR_TF_A}"
EXTDT_DIR_CONFIG += "virtual/bootloader:${EXTDT_DIR_UBOOT}"
EXTDT_DIR_CONFIG += "virtual/trusted-firmware-m:${EXTDT_DIR_TF_M}"
EXTDT_DIR_CONFIG += "virtual-optee-os:${EXTDT_DIR_OPTEE}"
EXTDT_DIR_CONFIG += "virtual/kernel:${EXTDT_DIR_LINUX}"

EXTDT_DIR_CONFIG += "virtual/lib64-trusted-firmware-a:${EXTDT_DIR_TF_A}"
EXTDT_DIR_CONFIG += "virtual/lib64-bootloader:${EXTDT_DIR_UBOOT}"
EXTDT_DIR_CONFIG += "virtual/lib64-trusted-firmware-m:${EXTDT_DIR_TF_M}"
EXTDT_DIR_CONFIG += "lib64-virtual-optee-os:${EXTDT_DIR_OPTEE}"

EXTDT_FILE_PATTERNS += ".*\.dts$"
EXTDT_FILE_PATTERNS += ".*\.dtsi$"
EXTDT_FILE_PATTERNS += "conf\.mk"
EXTDT_FILE_PATTERNS += "Makefile"

python __anonymous() {
    import re

    if d.getVar('EXTERNAL_DT_ENABLED') != "1":
        return

    package = d.getVar('BPN')
    if d.getVar('EXTDT_SRC_PROVIDER') == package:
        return

    found = False
    for extdt_conf in d.getVar('EXTDT_DIR_CONFIG').split():
        provider = extdt_conf.split(':')[0]
        sub_path = extdt_conf.split(':')[1]
        if provider in d.getVar('PROVIDES').split():
            found = True
            extdt_dir = os.path.join(d.getVar('STAGING_EXTDT_DIR'), sub_path)
            break
    if not found:
        bb.warn('[external-dt] No specific external-dt subfolder defined for %s recipe: the whole STAGING_EXTDT_DIR folder (%s) is used to feed CONFIGURE_FILES' % (package, d.getVar('STAGING_EXTDT_DIR')))
        bb.warn('[external-dt] Update EXTDT_DIR_CONFIG and set specific subfolder if needed:\n\tEXTDT_DIR_CONFIG: %s)' % d.getVar('EXTDT_DIR_CONFIG'))
        extdt_dir = d.getVar('STAGING_EXTDT_DIR')

    extdt_src_configure(d, extdt_dir)

    bb.debug(1,'[external-dt] Append file-checksums with configure files for do_configure on %s recipe' % package)
    d.appendVarFlag('do_configure', 'file-checksums', ' ${@extdt_srctree_configure_hash_files(d)}')
}

def extdt_srctree_configure_hash_files(d):
    """
    Get the list of files that should trigger do_configure to re-execute,
    based on the value of CONFIGURE_FILES
    """
    import fnmatch

    in_files = (d.getVar('CONFIGURE_FILES') or '').split()
    out_items = []
    search_files = []
    for entry in in_files:
        if entry.startswith('/'):
            out_items.append('%s:%s' % (entry, os.path.exists(entry)))
        else:
            search_files.append(entry)
    if search_files:
        s_dir = d.getVar('STAGING_EXTDT_DIR')
        for root, _, files in os.walk(s_dir):
            for p in search_files:
                for f in fnmatch.filter(files, p):
                    out_items.append('%s:True' % os.path.join(root, f))
    return ' '.join(out_items)

def extdt_src_configure(d, srcdir=None):
    import re
    extdt_dir = srcdir or ''
    package = d.getVar('BPN')
    # Update CONFIGURE_FILES according to external-dt subdir configured
    if os.path.exists(extdt_dir):
        dtfile_patterns = d.getVar('EXTDT_FILE_PATTERNS').split()
        configure_files = ''
        if d.getVar('EXTERNALSRC'):
            # In case of package managed through externalsrc class, consider default CONFIGURE_FILES
            configure_files = d.getVar('CONFIGURE_FILES') or ''
        for root, _, files in os.walk(extdt_dir):
            for f in files:
                for pattern in dtfile_patterns:
                    if re.match(pattern, f):
                        configure_files += ' ' + os.path.realpath(os.path.join(root, f))
                bb.debug(1, '[external-dt] Set CONFIGURE_FILES:pn-%s with files searched against proposed patterns (%s)' % (package, dtfile_patterns))
                d.setVar('CONFIGURE_FILES:pn-%s' % package, configure_files)

# Add dependency to get external-dt source code
do_compile[depends] += "${@bb.utils.contains('EXTERNAL_DT_ENABLED', '1', '${EXTDT_SRC_PROVIDER}:do_configure', '', d)}"
