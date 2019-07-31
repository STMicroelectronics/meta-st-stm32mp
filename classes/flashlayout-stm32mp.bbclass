# -----------------------------------------------------------------------------
# This class allows to output in deploy folder, along with the built image, the
# 'flashlayout_${PN}' sub-folder, populated with the flashlayout files to use
# with our STM32MP programmer tool to load the binaries in target device and
# partitions.
#
# There are two configurations:
#   * static:
#       The user provides the flashlayout files to export with the built image.
#   * dynamic:
#       The user configures the different variables to generate the expected
#       flashlayout files.
#
# --------------------
# Static configuration
# --------------------
# Set ENABLE_FLASHLAYOUT_DEFAULT to '1'.
# Configure FLASHLAYOUT_DEFAULT_SRC with the static flashlayout file locations.
#
# Configuration example (machine file or local.conf):
#   ENABLE_FLASHLAYOUT_DEFAULT = "1"
#   FLASHLAYOUT_DEFAULT_SRC = "files/flashlayouts/FlashLayout_sdcard_stm32mp157c-ev1_sample.tsv"
#
# ---------------------
# Dynamic configuration
# ---------------------
# Set ENABLE_FLASHLAYOUT_DEFAULT to '0'.
# In order to automatically generate flashlayout files as well formated TSV file
# there are some variables to configure.
#
# Naming:
#   <FLASHLAYOUT_BASENAME>[_<FLASHLAYOUT_CONFIG_LABEL>][_<FLASHLAYOUT_TYPE_LABEL>-FLASHLAYOUT_BOOTSCHEME_LABEL].<FLASHLAYOUT_SUFFIX>
#
#   FLASHLAYOUT_BASENAME
#       Default to 'FlashLayout'
#   FLASHLAYOUT_CONFIG_LABEL
#       Set from FLASHLAYOUT_CONFIG_LABELS list (without any '_' in config labels)
#   FLASHLAYOUT_TYPE_LABEL
#       Set from FLASHLAYOUT_TYPE_LABELS list
#   FLASHLAYOUT_BOOTSCHEME_LABEL
#       Set from FLASHLAYOUT_BOOTSCHEME_LABELS list (without any '_' in bootscheme labels)
# Note that both are appended only when FLASHLAYOUT_TYPE_LABELS and FLASHLAYOUT_BOOTSCHEME_LABELS contain more than two labels.
#   FLASHLAYOUT_SUFFIX
#       Default to 'tsv'
#
# File content structure:
#   Opt     Id      Name    Type    IP      Offset  Binary
#   <FLASHLAYOUT_PARTITION_ENABLE>
#           <FLASHLAYOUT_PARTITION_ID>
#                   <FLASHLAYOUT_PARTITION_LABEL>
#                           <FLASHLAYOUT_PARTITION_TYPE>
#                                   <FLASHLAYOUT_PARTITION_DEVICE>
#                                           <FLASHLAYOUT_PARTITION_OFFSET>
#                                                   <FLASHLAYOUT_PARTITION_BIN2LOAD>
#
# Specific configuration:
#   FLASHLAYOUT_PARTITION_SIZE
#       If configured, it allows to compute the next offset to apply in
#       flashlayout file for the following partition.
#       Note that according to the device in use for the partition a specific
#       alignment size can be specified through DEVICE_ALIGNMENT_SIZE_<device>
#       var where <device> is the current FLASHLAYOUT_PARTITION_DEVICE
#
# Note that override is manage for 'FLASHLAYOUT_PARTITION_LABELS' list with:
#   - <bootscheme-label> from FLASHLAYOUT_BOOTSCHEME_LABELS' list
#   - <config-label> from 'FLASHLAYOUT_CONFIG_LABELS' list
# Priority assignment is:
#   It means the 'FLASHLAYOUT_PARTITION_LABELS' value can be overriden by setting:
#   FLASHLAYOUT_PARTITION_LABELS_<bootscheme-label>_<config-label>
#   FLASHLAYOUT_PARTITION_LABELS_<bootscheme-label>
#   FLASHLAYOUT_PARTITION_LABELS_<config-label>
#   FLASHLAYOUT_PARTITION_LABELS
#
# Another override mechanism is also implemented for all other partition variables:
#   FLASHLAYOUT_PARTITION_ENABLE
#   FLASHLAYOUT_PARTITION_ID
#   FLASHLAYOUT_PARTITION_TYPE
#   FLASHLAYOUT_PARTITION_DEVICE
#   FLASHLAYOUT_PARTITION_OFFSET
#   FLASHLAYOUT_PARTITION_BIN2LOAD
# We can override these variable with:
#   - <config-label> from 'FLASHLAYOUT_CONFIG_LABELS' list
#   - <bootscheme-label> from 'FLASHLAYOUT_BOOTSCHEME_LABELS' list
#   - <partition-label> from 'FLASHLAYOUT_PARTITION_LABELS' list
# Priority assignment is:
#   FLASHLAYOUT_PARTITION_xxx_<bootscheme-label>_<config-label>_<partition-label>
#   FLASHLAYOUT_PARTITION_xxx_<bootscheme-label>_<config-label>
#   FLASHLAYOUT_PARTITION_xxx_<bootscheme-label>_<partition-label>
#   FLASHLAYOUT_PARTITION_xxx_<bootscheme-label>
#   FLASHLAYOUT_PARTITION_xxx_<config-label>_<partition-label>
#   FLASHLAYOUT_PARTITION_xxx_<config-label>
#   FLASHLAYOUT_PARTITION_xxx_<partition-label>
#   FLASHLAYOUT_PARTITION_xxx
# -----------------------------------------------------------------------------

ENABLE_FLASHLAYOUT_CONFIG ??= "1"

FLASHLAYOUT_SUBDIR  = "flashlayout_${PN}"
FLASHLAYOUT_DESTDIR = "${IMGDEPLOYDIR}/${FLASHLAYOUT_SUBDIR}"

FLASHLAYOUT_BASENAME ??= "FlashLayout"
FLASHLAYOUT_SUFFIX   ??= "tsv"

FLASHLAYOUT_BOOTSCHEME_LABELS ??= ""

ENABLE_FLASHLAYOUT_DEFAULT ??= "0"
FLASHLAYOUT_DEFAULT_SRC ??= ""

# List all specific dependencies to image_complete task for successfull build
FLASHLAYOUT_DEPEND_TASKS ??= ""

# List configuration files to monitor to trigger new flashlayout generation
FLASHLAYOUT_CONFIGURE_FILES ??= ""

# -----------------------------------------------------------------------------
# Make sure to add the flashlayout file creation after ROOTFS build
# So we should identify image ROOTFS build and only the ROOTFS (for now)
# As we know that PARTITIONS may be built as part of ROOTFS build, let's
# avoid amending the partition images
# -----------------------------------------------------------------------------
python __anonymous () {
    flashlayout_config = d.getVar('ENABLE_FLASHLAYOUT_CONFIG')
    if flashlayout_config:
        # Gather all current tasks
        tasks = filter(lambda k: d.getVarFlag(k, "task", True), d.keys())
        for task in tasks:
            # Check that we are dealing with image recipe
            if task == 'do_image_complete':
                # Init current image name
                current_image_name = d.getVar('PN') or ""
                # Init RAMFS image if any
                initramfs = d.getVar('INITRAMFS_IMAGE') or ""
                # Init INITRD image if any
                initrd = d.getVar('INITRD_IMAGE') or ""
                # Init partition list from PARTITIONS_IMAGE
                image_partitions = (d.getVar('PARTITIONS_IMAGE') or "").split()
                # We need to clearly identify ROOTFS build, not InitRAMFS one (if any)
                if current_image_name not in image_partitions and current_image_name not in initramfs and current_image_name not in initrd:
                    # We need to make sure to add all extra dependencies as 'depends'
                    # for image_complete task
                    if d.getVar('FLASHLAYOUT_DEPEND_TASKS'):
                        d.appendVarFlag('do_image_complete', 'depends', ' %s' % (d.getVar('FLASHLAYOUT_DEPEND_TASKS',)))
                    # We can append the flashlayout file creation task to this ROOTFS build
                    d.appendVar('IMAGE_POSTPROCESS_COMMAND', 'do_create_flashlayout_config ; ')
                    # Append also the configuration files to properly take into account any updates
                    d.appendVarFlag('do_image_complete', 'file-checksums', ' ${FLASHLAYOUT_CONFIGURE_FILES} ')
}

def expand_var(var, bootscheme, config, partition, d):
    """
    Compute and return 'var':
        0) Prepend 'partition' to default OVERRIDES
        1) Look for any 'bootscheme_config' expansion for 'var': 'var_bootscheme_config'
        2) Look for any 'bootscheme' expansion for 'var': 'var_bootscheme'
        3) Look for any 'config' expansion for 'var': 'var_config'
        4) Then look for any 'var' override
        5) Default 'var' to 'none' if not defined
    This is the priority order assignment for 'var'
    """
    # Append 'partition' to OVERRIDES
    localdata = bb.data.createCopy(d)
    overrides = localdata.getVar('OVERRIDES')
    if not overrides:
        bb.fatal('OVERRIDES not defined')
    localdata.setVar('OVERRIDES', partition + ':' + overrides)
    # Compute var according to priority assignment order defined above
    expanded_var = localdata.getVar('%s_%s_%s' % (var, bootscheme, config))
    if not expanded_var:
        expanded_var = localdata.getVar('%s_%s' % (var, bootscheme))
    if not expanded_var:
        expanded_var = localdata.getVar('%s_%s' % (var, config))
    if not expanded_var:
        expanded_var = localdata.getVar(var)
    if not expanded_var:
        expanded_var = "none"
    # Return expanded and/or overriden var value
    return expanded_var

def get_offset(new_offset, bootscheme, config, partition, d):
    """
    This function return a couple of strings: offset, next_offset
    The offset is the one to use in flashlayout file for the requested partition,
    and next_offset is the one to use in flashlayout for next partition (if any).

    The offset can be directly configured for the current partition through the
    FLASHLAYOUT_PARTITION_OFFSET variable. If this one is set to 'none' for the
    current partition, then we use the one provided through 'new_offset'.

    The next_offset is computed by first getting the FLASHLAYOUT_PARTITION_SIZE for
    the current partition, and we make sure to align properly the next_offset
    according to the DEVICE_ALIGNMENT_SIZE_<device> where <device> is feed from
    FLASHLAYOUT_PARTITION_DEVICE.
    """
    import re

    # Set offset
    offset = expand_var('FLASHLAYOUT_PARTITION_OFFSET', bootscheme, config, partition, d)
    bb.note('>>> Selected FLASHLAYOUT_PARTITION_OFFSET: %s' % offset)
    if offset == 'none':
        if new_offset == 'none':
            bb.fatal('Missing %s partition offset configuration for %s label for %s bootscheme!' % (partition, config, bootscheme))
        offset = new_offset
        bb.note('>>> New offset configured: %s' % offset)

    # Set next offset
    partition_size = expand_var('FLASHLAYOUT_PARTITION_SIZE', bootscheme, config, partition, d)
    bb.note('>>> Selected FLASHLAYOUT_PARTITION_SIZE: %s' % partition_size)
    if not partition_size.isdigit():
        bb.note('No partition size provided for %s partition, %s label and %s bootscheme!' % (partition, config, bootscheme))
        next_offset = "none"
    else:
        if re.match('^0x.*$', offset):
            current_device = expand_var('FLASHLAYOUT_PARTITION_DEVICE', bootscheme, config, partition, d)
            bb.note('>>> Current device is %s' % current_device)
            alignment_size = d.getVar('DEVICE_ALIGNMENT_SIZE_%s' % current_device) or "none"
            if alignment_size == 'none':
                bb.fatal('Missing DEVICE_ALIGNMENT_SIZE_%s value' % current_device)
            if ( int(partition_size) * 1024 ) % int(alignment_size, 16) == 0:
                bb.note('>>> The partition size properly follows %s erase size' % alignment_size)
            else:
                bb.note('>>> The %s alignment size is: %s' % (current_device, alignment_size))
                floor_coef = ( int(partition_size) * 1024 ) // int(alignment_size, 16)
                compute_size = ( floor_coef + 1 ) * int(alignment_size, 16)
                partition_size = str(compute_size // 1024)
                bb.note('>>> New partition size configured to follow %s alignment size: %s' % (alignment_size, partition_size))
            # Compute new offset value
            overall_size = int(offset, 16) + int(partition_size) * 1024
            next_offset = '0x{0:0{1}X}'.format(overall_size, 8)
        else:
            next_offset = "none"
    bb.note('>>> New next_offset configured: %s' % next_offset)

    # Return both offset and next offset
    return str(offset), str(next_offset)


def get_binaryname(labeltype, bootscheme, config, partition, d):
    """
    Return proper binary name to use in flashlayout file by applying any specific
    computation (replacement, etc)
    Make sure also that binary is available on deploy folder
    """
    import re

    # Init binary_name for current configuration
    binary_name = expand_var('FLASHLAYOUT_PARTITION_BIN2LOAD', bootscheme, config, partition, d)
    bb.note('>>> Selected FLASHLAYOUT_PARTITION_BIN2LOAD: %s' % binary_name)

    # Treat TF-A, TEE, U-BOOT and U-BOOT-SPL binary rename case
    if re.match('^tf-a.*$', binary_name) or re.match('^u-boot.*$', binary_name) or re.match('^tee-.*$', binary_name):
        file_name, file_ext = os.path.splitext(binary_name)
        # Init binary_type to use from labeltype
        binary_type = labeltype + '-' + bootscheme
        bb.note('>>> Binary type used: %s' % binary_type)
        # Check for any replace pattern
        replace_patterns = expand_var('BIN2BOOT_REPLACE_PATTERNS', bootscheme, config, partition, d)
        bb.note('>>> Substitution patterns: %s' % replace_patterns)
        # Apply replacement patterns on binary_type
        if replace_patterns != 'none':
            for replace_pattern in replace_patterns.split():
                pattern2replace = replace_pattern.split(';')[0]
                pattern2use = replace_pattern.split(';')[1]
                # Replace with pattern middle of string
                binary_type = re.sub(r'-%s-' % pattern2replace, '-' + pattern2use + '-', binary_type)
                # Replace with pattern end of string
                binary_type = re.sub(r'-%s$' % pattern2replace, '-' + pattern2use, binary_type)
            bb.note('>>> New "binary_type" to use for binary name": %s' % binary_type)
        # Append binary_type to binary name
        if re.match('^u-boot-spl.*$', binary_name):
            binary_name = file_name + file_ext + '-' + binary_type
        else:
            binary_name = file_name + '-' + binary_type + file_ext

    # Make sure binary is available in DEPLOY_DIR_IMAGE folder
    if binary_name != 'none':
        if not os.path.isfile(os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), binary_name)):
            # Specific exception for rootfs binary (not yet deployed)
            if not os.path.isfile(os.path.join(d.getVar('IMGDEPLOYDIR'), binary_name)):
                bb.fatal('Missing %s binary file in deploy folder' % binary_name)
    # Return binary_name value
    return binary_name

def flashlayout_search(d, files):
    search_path = d.getVar("BBPATH").split(":")
    for file in files.split():
        for p in search_path:
            file_path = p + "/" + file
            if os.path.isfile(file_path):
                return (True, file_path)
    return (False, "")

python do_create_flashlayout_config() {
    import re
    import shutil

    # We check first if it is requested to generate any flashlayout files
    if d.getVar("ENABLE_FLASHLAYOUT_CONFIG") != "1":
        bb.note('ENABLE_FLASHLAYOUT_CONFIG not enabled')
        return

    # Create destination folder for flashlayout files
    bb.utils.remove(d.getVar('FLASHLAYOUT_DESTDIR'), recurse=True)
    bb.utils.mkdirhier(d.getVar('FLASHLAYOUT_DESTDIR'))

    # We check if user as define a static flashlayout file to use instead of dynamic generation
    if d.getVar("ENABLE_FLASHLAYOUT_DEFAULT") == "1":
        bb.note('ENABLE_FLASHLAYOUT_DEFAULT enabled')
        flashlayout_src = d.getVar("FLASHLAYOUT_DEFAULT_SRC")
        if not flashlayout_src:
            bb.fatal("FLASHLAYOUT_DEFAULT_SRC not defined, please set a proper value")
        if not flashlayout_src.strip():
            bb.fatal("No static flashlayout file configured, nothing to do")
        found, f = flashlayout_search(d, flashlayout_src)
        if found:
            flashlayout_staticname=os.path.basename(f)
            flashlayout_file = d.expand("${FLASHLAYOUT_DESTDIR}/%s" % flashlayout_staticname)
            shutil.copy2(f, flashlayout_file)
            bb.note('Copy %s to output file %s' % (f, flashlayout_file))
            return
        else:
            bb.fatal("Configure static file: %s not found" % flashlayout_src)

    # Set bootschemes for partition var override configuration
    bootschemes = d.getVar('FLASHLAYOUT_BOOTSCHEME_LABELS')
    if not bootschemes:
        bb.fatal("FLASHLAYOUT_BOOTSCHEME_LABELS not defined, nothing to do")
    if not bootschemes.strip():
        bb.fatal("No bootschemes, nothing to do")
    # Make sure there is no '_' in FLASHLAYOUT_BOOTSCHEME_LABELS
    for bootscheme in bootschemes.split():
        if re.match('.*_.*', bootscheme):
            bb.fatal("Please remove all '_' for bootschemes defined in FLASHLAYOUT_BOOTSCHEME_LABELS")
    bb.note('FLASHLAYOUT_BOOTSCHEME_LABELS: %s' % bootschemes)

    for bootscheme in bootschemes.split():
        bb.note('*** Loop for bootscheme label: %s' % bootscheme)

        # Get the different flashlayout config label
        configs = expand_var('FLASHLAYOUT_CONFIG_LABELS', bootscheme, '', '', d)
        # Make sure there is no '_' in FLASHLAYOUT_CONFIG_LABELS
        for config in configs.split():
            if re.match('.*_.*', config):
                bb.fatal("Please remove all '_' for configs defined in FLASHLAYOUT_CONFIG_LABELS")
        bb.note('FLASHLAYOUT_CONFIG_LABELS: %s' % configs)

        for config in configs.split():
            bb.note('*** Loop for config label: %s' % config)
            # Set labeltypes list
            labeltypes = expand_var('FLASHLAYOUT_TYPE_LABELS', bootscheme, config, '', d)
            bb.note('FLASHLAYOUT_TYPE_LABELS: %s' % labeltypes)
            if labeltypes == 'none':
                bb.note("FLASHLAYOUT_TYPE_LABELS is none, so no flashlayout file to generate.")
                continue
            for labeltype in labeltypes.split():
                bb.note('*** Loop for label type: %s' % labeltype)
                # Init flashlayout file name
                if config == 'none':
                    config_append = ''
                else:
                    config_append = '_' + config
                if len(labeltypes.split()) < 2 and len(bootschemes.split()) < 2:
                    labeltype_append = ''
                else:
                    labeltype_append = '_' + labeltype + '-' + bootscheme
                flashlayout_file = d.expand("${FLASHLAYOUT_DESTDIR}/${FLASHLAYOUT_BASENAME}%s%s.${FLASHLAYOUT_SUFFIX}" % (config_append, labeltype_append))
                # Get the partition list to write in flashlayout file
                partitions = expand_var('FLASHLAYOUT_PARTITION_LABELS', bootscheme, config, '', d)
                bb.note('FLASHLAYOUT_PARTITION_LABELS: %s' % partitions)
                if partitions == 'none':
                    bb.note("FLASHLAYOUT_PARTITION_LABELS is none, so no flashlayout file to generate.")
                    continue
                # Generate flashlayout file for labeltype
                try:
                    with open(flashlayout_file, 'w') as fl_file:
                        # Write to flashlayout file the first line header
                        fl_file.write('#Opt\tId\tName\tType\tIP\tOffset\tBinary\n')
                        # Init partition next offset to 'none'
                        partition_nextoffset = "none"
                        for partition in partitions.split():
                            bb.note('*** Loop for partition: %s' % partition)
                            # Init partition settings
                            partition_enable = expand_var('FLASHLAYOUT_PARTITION_ENABLE', bootscheme, config, partition, d)
                            partition_id = expand_var('FLASHLAYOUT_PARTITION_ID', bootscheme, config, partition, d)
                            partition_name = partition
                            partition_type = expand_var('FLASHLAYOUT_PARTITION_TYPE', bootscheme, config, partition, d)
                            partition_device = expand_var('FLASHLAYOUT_PARTITION_DEVICE', bootscheme, config, partition, d)
                            # Get partition offset
                            partition_offset, partition_nextoffset = get_offset(partition_nextoffset, bootscheme, config, partition, d)
                            # Get binary name
                            partition_bin2load = get_binaryname(labeltype, bootscheme, config, partition, d)
                            # Be verbose in log file
                            bb.note('>>> Layout inputs: %s' % fl_file.name)
                            bb.note('>>> FLASHLAYOUT_PARTITION_ENABLE:      %s' % partition_enable)
                            bb.note('>>> FLASHLAYOUT_PARTITION_ID:          %s' % partition_id)
                            bb.note('>>> FLASHLAYOUT_PARTITION_LABEL:       %s' % partition_name)
                            bb.note('>>> FLASHLAYOUT_PARTITION_TYPE:        %s' % partition_type)
                            bb.note('>>> FLASHLAYOUT_PARTITION_DEVICE:      %s' % partition_device)
                            bb.note('>>> FLASHLAYOUT_PARTITION_OFFSET:      %s' % partition_offset)
                            bb.note('>>> FLASHLAYOUT_PARTITION_BIN2LOAD:    %s' % partition_bin2load)
                            bb.note('>>> done')
                            # Write to flashlayout file the partition configuration
                            fl_file.write('%s\t%s\t%s\t%s\t%s\t%s\t%s\n' %
                                         (partition_enable, partition_id, partition_name, partition_type, partition_device, partition_offset, partition_bin2load))
                except OSError:
                    bb.fatal('Unable to open %s' % (fl_file))
}
