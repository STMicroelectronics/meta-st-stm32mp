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

# Configure flashlayout file generation
ENABLE_FLASHLAYOUT_CONFIG ??= "1"
# Configure direct use of flashlayout file without automatic file generation
ENABLE_FLASHLAYOUT_DEFAULT ??= "0"
# Configure path for provided flashlayout file
FLASHLAYOUT_DEFAULT_SRC ??= ""
# Configure flashlayout file name default format
FLASHLAYOUT_BASENAME ??= "FlashLayout"
FLASHLAYOUT_SUFFIX   ??= "tsv"
# Configure flashlayout file generation for stm32wrapper4dbg
ENABLE_FLASHLAYOUT_CONFIG_WRAPPER4DBG ??= "0"

# Configure folders for flashlayout file generation
FLASHLAYOUT_DEPLOYDIR ?= "${DEPLOY_DIR}/images/${MACHINE}"
FLASHLAYOUT_TOPDIR ?= "${WORKDIR}/flashlayout-destdir/"
FLASHLAYOUT_SUBDIR ?= "flashlayout_${PN}"
FLASHLAYOUT_DESTDIR = "${FLASHLAYOUT_TOPDIR}/${FLASHLAYOUT_SUBDIR}"

# Init bootscheme and config labels
FLASHLAYOUT_BOOTSCHEME_LABELS ??= ""
FLASHLAYOUT_CONFIG_LABELS ??= ""
# Init partition image list (used to configure partitions)
FLASHLAYOUT_PARTITION_IMAGES ??= ""
# Init partition and type labels
#   Note: possible override with bootscheme and/or config
FLASHLAYOUT_PARTITION_LABELS   ??= ""
FLASHLAYOUT_TYPE_LABELS ??= ""
# Init flashlayout partition vars
#   Note: possible override with bootscheme and/or config and/or partition
FLASHLAYOUT_PARTITION_ENABLE ??= ""
FLASHLAYOUT_PARTITION_ID ??= ""
FLASHLAYOUT_PARTITION_TYPE ??= ""
FLASHLAYOUT_PARTITION_DEVICE ??= ""
FLASHLAYOUT_PARTITION_OFFSET ??= ""
FLASHLAYOUT_PARTITION_BIN2LOAD ??= ""
FLASHLAYOUT_PARTITION_SIZE ??= ""
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS ??= ""

python __anonymous () {
    # -----------------------------------------------------------------------------
    # Make sure to add the flashlayout file creation after ROOTFS build
    # So we should identify image ROOTFS build and only the ROOTFS (for now)
    # As we know that PARTITIONS may be built as part of ROOTFS build, let's
    # avoid amending the partition images
    # -----------------------------------------------------------------------------
    if d.getVar('ENABLE_FLASHLAYOUT_CONFIG') == "1":
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
                # Init partition list from PARTITIONS_CONFIG
                image_partitions = []
                # Append image_partitions list with all configured partition images:

                partitionsconfigflags = d.getVarFlags('PARTITIONS_CONFIG')
                # The "doc" varflag is special, we don't want to see it here
                partitionsconfigflags.pop('doc', None)
                partitionsconfig = (d.getVar('PARTITIONS_CONFIG') or "").split()
                if len(partitionsconfig) > 0:
                    for config in partitionsconfig:
                        for f, v in partitionsconfigflags.items():
                            if config == f:
                                items = v.split(',')
                                # Make sure about PARTITIONS_CONFIG contents
                                if items[0] and len(items) > 5:
                                    bb.fatal('[PARTITIONS_CONFIG] Only image,label,mountpoint,size,type can be specified!')
                                # Make sure that we're dealing with partition image and not rootfs image
                                if len(items) > 2 and items[2]:
                                    # Mount point is available, so we're dealing with partition image
                                    # Append image to image_partitions list
                                    image_partitions.append(d.expand(items[0]))
                                break

                # We need to clearly identify ROOTFS build, not InitRAMFS/initRD one (if any), not partition one either
                if current_image_name not in image_partitions and current_image_name != initramfs and current_image_name != initrd:
                    # We add the flashlayout file creation task just after the do_image_complete for ROOTFS build
                    bb.build.addtask('do_create_flashlayout_config', 'do_build', 'do_image_complete', d)
                    # We add also the function that feeds the FLASHLAYOUT_PARTITION_* vars from PARTITIONS_CONFIG
                    d.appendVarFlag('do_create_flashlayout_config', 'prefuncs', ' flashlayout_partition_image_config')
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

def get_device(bootscheme, config, partition, d):
    """
    This function returns the device configured from FLASHLAYOUT_PARTITION_DEVICE for
    the requested partition for label and bootscheme configured.
    The var FLASHLAYOUT_PARTITION_DEVICE can be configured through different scheme
        FLASHLAYOUT_PARTITION_DEVICE = '<device0>:<dev0part_0> <dev0part_1>,<device1>:<dev1part0>'
        FLASHLAYOUT_PARTITION_DEVICE = '<device0>:default,<device1>:<dev1part0> <dev1part1>,<device2>:<dev2part0>'
        FLASHLAYOUT_PARTITION_DEVICE = '<device0>'
    Then, to set the device for the current partition, the logic followed is:
        If the configuration provides a single device, then partition device is set
        to this value.
        Else,
            If the current partition is specified in any of the configured partition
            lists, the matching configured device is set as partition device.
            And if the current partition is not found, the default device configured
            is set as partition device.
    """
    # Set device configuration
    device_configs = expand_var('FLASHLAYOUT_PARTITION_DEVICE', bootscheme, config, partition, d)
    bb.note('>>> Selected FLASHLAYOUT_PARTITION_DEVICE: %s' % device_configs)

    if len(device_configs.split(',')) == 1:
        bb.note('>>> Only one device configuration set for %s partition for %s label for %s bootscheme' % (partition, config, bootscheme))
        device = device_configs.split(':')[0]
    else:
        bb.note('>>> Multiple device configurations set for %s partition for %s label for %s bootscheme' % (partition, config, bootscheme))
        # Init default_device and device to empty string
        default_device = ''
        device = ''
        for device_config in device_configs.split(','):
            cfg_devc = device_config.split(':')[0].strip()
            cfg_part = device_config.split(':')[1] or 'default'
            # Make sure configuration is correct
            if len(cfg_devc.split()) > 1:
                bb.fatal('Only one device configuration can be specified: found %s for %s partition for %s label for %s bootscheme' % (cfg_devc, partition, config, bootscheme))
            # Configure the default device configuration if any
            if cfg_part == 'default':
                if default_device != '':
                    bb.fatal('Found two "default" device configuration for %s partition for %s label for %s bootscheme in FLASHLAYOUT_PARTITION_DEVICE var' % (partition, config, bootscheme))
                default_device = cfg_devc
                bb.note('>>> Set default device configuration to %s' % default_device)
            else:
                # Find out if any device is configured for current partition
                for p in cfg_part.split():
                    if p == partition:
                        device = cfg_devc
                        break
        # If 'device' is still empty for current partition, check if we can apply default device configuration
        if device == '':
            if default_device == '':
                bb.fatal('Not able to get device configuration for %s partition for %s label for %s bootscheme' % (partition, config, bootscheme))
            else:
                bb.note('>>> Configure device to default device setting')
                device = default_device
    bb.note('>>> New device configured: %s' % device)
    # Return the value computed
    return device

def get_offset(new_offset, current_device, bootscheme, config, partition, d):
    """
    This function returns a couple of strings: offset, next_offset
    The offset is the one to use in flashlayout file for the requested partition,
    and next_offset is the one to use in flashlayout for next partition (if any).

    The offset can be directly configured for the current partition through the
    FLASHLAYOUT_PARTITION_OFFSET variable. If this one is set to 'none' for the
    current partition, then we use the one provided through 'new_offset' if set,
    else we default to DEVICE_START_OFFSET_<device> one where <device> is feed from
    'current_device' input.

    The next_offset is computed by first getting the FLASHLAYOUT_PARTITION_SIZE for
    the current partition, and we make sure to align properly the next_offset
    according to the DEVICE_ALIGNMENT_SIZE_<device> where <device> is feed from
    'current_device' input.
    """
    import re

    # Get current_device alias
    device_alias = d.getVar('DEVICE_%s' % current_device) or ""

    # Set offset
    offset = expand_var('FLASHLAYOUT_PARTITION_OFFSET', bootscheme, config, partition, d)
    bb.note('>>> Selected FLASHLAYOUT_PARTITION_OFFSET: %s' % offset)
    if offset == 'none':
        if new_offset == 'none':
            bb.note('>>> No %s partition offset configured (%s device) for %s label for %s bootscheme, so default to default origin device one.' % (partition, current_device, config, bootscheme))
            start_offset = d.getVar('DEVICE_START_OFFSET_%s' % device_alias) or "none"
            if start_offset == 'none':
                bb.fatal('Missing DEVICE_START_OFFSET_%s value' % device_alias)
            offset = start_offset
        else:
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
            bb.note('>>> Current device is %s (%s alias)' % (current_device, device_alias))
            alignment_size = d.getVar('DEVICE_ALIGNMENT_SIZE_%s' % device_alias) or "none"
            if alignment_size == 'none':
                bb.fatal('Missing DEVICE_ALIGNMENT_SIZE_%s value' % device_alias)
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

    # Get binary_name basename to then check for any rename case
    binary_name_base = os.path.basename(binary_name)
    bb.note('>>> Basename selected for %s: %s' % (binary_name, binary_name_base))

    # Treat TF-A, TEE, U-BOOT and U-BOOT-SPL binary rename case
    if re.match('^tf-a.*$', binary_name_base) or re.match('^u-boot.*$', binary_name_base) or re.match('^tee-.*$', binary_name_base):
        file_name, file_ext = os.path.splitext(binary_name)
        # Init binary_type to use from labeltype
        binary_type = labeltype + '-' + bootscheme
        bb.note('>>> Binary type used: %s' % binary_type)
        # Check for any replace pattern
        replace_patterns = expand_var('FLASHLAYOUT_PARTITION_REPLACE_PATTERNS', bootscheme, config, partition, d)
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
        if re.match('^u-boot-spl.*$', binary_name_base):
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
            file_path = os.path.join(p, file)
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
        for fl_src in flashlayout_src.split():
            found, f = flashlayout_search(d, fl_src)
            if found:
                flashlayout_staticname=os.path.basename(f)
                flashlayout_file = os.path.join(d.getVar('FLASHLAYOUT_DESTDIR'), flashlayout_staticname)
                shutil.copy2(f, flashlayout_file)
                bb.note('Copy %s to output file %s' % (f, flashlayout_file))
            else:
                bb.fatal("Configure static file: %s not found" % fl_src)
        return

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

        if configs.strip() == 'none':
            bb.note("FLASHLAYOUT_CONFIG_LABELS is none, so no flashlayout file to generate.")
            continue
        # Create bootscheme subfolder for flashlayout files
        flashlayout_subfolder_path = os.path.join(d.getVar('FLASHLAYOUT_DESTDIR'), bootscheme)
        bb.utils.mkdirhier(flashlayout_subfolder_path)

        for config in configs.split():
            bb.note('*** Loop for config label: %s' % config)
            # Set labeltypes list
            labeltypes = expand_var('FLASHLAYOUT_TYPE_LABELS', bootscheme, config, '', d)
            bb.note('FLASHLAYOUT_TYPE_LABELS: %s' % labeltypes)
            if labeltypes.strip() == 'none':
                bb.note("FLASHLAYOUT_TYPE_LABELS is none, so no flashlayout file to generate.")
                continue
            for labeltype in labeltypes.split():
                bb.note('*** Loop for label type: %s' % labeltype)
                # Init current label
                current_label = labeltype
                # Init flashlayout file name
                if config == 'none':
                    config_append = ''
                else:
                    config_append = '_' + config
                if len(labeltypes.split()) < 2 and len(bootschemes.split()) < 2:
                    labeltype_append = ''
                else:
                    labeltype_append = '_' + labeltype + '-' + bootscheme
                flashlayout_file = os.path.join(flashlayout_subfolder_path, d.expand("${FLASHLAYOUT_BASENAME}%s%s.${FLASHLAYOUT_SUFFIX}" % (config_append, labeltype_append)))
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
                        # Init partition previous device to 'none'
                        partition_prevdevice = "none"
                        for partition in partitions.split():
                            bb.note('*** Loop for partition: %s' % partition)
                            # Init partition settings
                            partition_enable = expand_var('FLASHLAYOUT_PARTITION_ENABLE', bootscheme, config, partition, d)
                            partition_id = expand_var('FLASHLAYOUT_PARTITION_ID', bootscheme, config, partition, d)
                            partition_name = partition
                            partition_type = expand_var('FLASHLAYOUT_PARTITION_TYPE', bootscheme, config, partition, d)
                            partition_device = get_device(bootscheme, config, partition, d)
                            # Reset partition_nextoffset to 'none' in case partition device has changed
                            if partition_device != partition_prevdevice:
                                partition_nextoffset = "none"
                            # Save partition current device to previous one for next loop
                            partition_prevdevice = partition_device
                            # Get partition offset
                            partition_offset, partition_nextoffset = get_offset(partition_nextoffset, partition_device, bootscheme, config, partition, d)
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
                            # Get the supported labels for current storage device
                            partition_device_alias = d.getVar('DEVICE_%s' % partition_device) or ""
                            partition_type_supported_labels = d.getVar('DEVICE_BOARD_ENABLE_%s' % partition_device_alias) or "none"
                            # Check if partition type is supported for the current label
                            if partition_device != 'none' and current_label not in partition_type_supported_labels.split():
                                bb.note('>>> FLASHLAYOUT_PARTITION_DEVICE (%s, alias %s) is not supported for current label (%s): partition %s not appended in flashlayout file' % (partition_device, partition_device_alias, current_label, partition_name))
                                bb.note('>>> DEVICE_BOARD_ENABLE_%s: %s' % (partition_device_alias, partition_type_supported_labels))
                                continue
                            # Write to flashlayout file the partition configuration
                            fl_file.write('%s\t%s\t%s\t%s\t%s\t%s\t%s\n' %
                                         (partition_enable, partition_id, partition_name, partition_type, partition_device, partition_offset, partition_bin2load))
                except OSError:
                    bb.fatal('Unable to open %s' % (fl_file))

                if d.getVar("ENABLE_FLASHLAYOUT_CONFIG_WRAPPER4DBG") == "1":
                    bb.note('*** Loop for flashlayout for the wrapper for debug %s' % labeltype)

                    tmp_flashlayout_file = os.path.join(flashlayout_subfolder_path, "flashlayout.tmp")
                    debug_flashlayout = False

                    try:
                        with open(flashlayout_file, 'r') as fl_file:
                            try:
                                with open(tmp_flashlayout_file, 'w') as debug_fl_file:
                                    for line in fl_file:
                                        if re.match('^.*/tf-a.*$', line) :
                                            line_tmp = re.sub(r'(.*)/',r'\1/debug/debug-', line)
                                            filename = re.sub(r'.*[\t ](.*)$',r'\1', line_tmp).strip()
                                            if os.path.isfile(os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), filename)):
                                                line = line_tmp
                                                debug_flashlayout = True

                                        debug_fl_file.write('%s' % (line))
                            except OSError:
                                bb.fatal('Unable to open %s' % (debug_fl_file))
                    except OSError:
                        bb.fatal('Unable to open %s' % (fl_file))
                    if debug_flashlayout:
                        flashlayout_wrapper4dbg_subfolder_path = os.path.join(d.getVar('FLASHLAYOUT_DESTDIR'), bootscheme, "debug")
                        bb.utils.mkdirhier(flashlayout_wrapper4dbg_subfolder_path)
                        # Wrapper4dbg output filename
                        debug_flashlayout_file = os.path.join(flashlayout_wrapper4dbg_subfolder_path,d.expand("debug-${FLASHLAYOUT_BASENAME}%s%s.${FLASHLAYOUT_SUFFIX}" % (config_append, labeltype_append)))
                        bb.note(">>> Update tf-a in %s" %  (debug_flashlayout_file))
                        os.rename(tmp_flashlayout_file, debug_flashlayout_file)
                    else:
                        os.remove(tmp_flashlayout_file)
}
do_create_flashlayout_config[dirs] = "${FLASHLAYOUT_DESTDIR}"

FLASHLAYOUT_DEPEND_TASKS ?= ""
do_create_flashlayout_config[depends] += "${FLASHLAYOUT_DEPEND_TASKS}"

SSTATETASKS += "do_create_flashlayout_config"
do_create_flashlayout_config[cleandirs] = "${FLASHLAYOUT_TOPDIR}"
do_create_flashlayout_config[sstate-inputdirs] = "${FLASHLAYOUT_TOPDIR}"
do_create_flashlayout_config[sstate-outputdirs] = "${FLASHLAYOUT_DEPLOYDIR}/"

python do_create_flashlayout_config_setscene () {
    sstate_setscene(d)
}
addtask do_create_flashlayout_config_setscene

python flashlayout_partition_image_config() {
    """
    Set the different flashlayout partition vars for the configure partition
    images.
    Based on PARTITIONS_CONFIG, feed:
        FLASHLAYOUT_PARTITION_IMAGES
        FLASHLAYOUT_PARTITION_ID_
        FLASHLAYOUT_PARTITION_TYPE_
        FLASHLAYOUT_PARTITION_SIZE_
        FLASHLAYOUT_PARTITION_BIN2LOAD_
    """

    partitionsconfigflags = d.getVarFlags('PARTITIONS_CONFIG')
    # The "doc" varflag is special, we don't want to see it here
    partitionsconfigflags.pop('doc', None)
    partitionsconfig = (d.getVar('PARTITIONS_CONFIG') or "").split()

    if len(partitionsconfig) > 0:
        # Init default partition id for binary type and other
        id_bin = 4
        id_oth = 33
        for config in partitionsconfig:
            for f, v in partitionsconfigflags.items():
                if config == f:
                    items = v.split(',')
                    # Make sure about PARTITIONS_CONFIG contents
                    if items[0] and len(items) > 5:
                        bb.fatal('[PARTITIONS_CONFIG] Only image,label,mountpoint,size,type can be specified!')
                    if items[1]:
                        bb.debug(1, "Appending %s to FLASHLAYOUT_PARTITION_IMAGES." % items[1])
                        d.appendVar('FLASHLAYOUT_PARTITION_IMAGES', ' ' + items[1])
                    else:
                        bb.fatal('[PARTITIONS_CONFIG] Missing image label setting')
                    # Init flashlayout label
                    fl_label = d.expand(items[1])
                    if items[2] == '':
                        # There is no mountpoint specified, so we apply rootfs image format
                        bb.debug(1, "Set FLASHLAYOUT_PARTITION_BIN2LOAD_%s to %s." % (fl_label, items[0] + "-${MACHINE}.ext4"))
                        d.setVar('FLASHLAYOUT_PARTITION_BIN2LOAD_%s' % fl_label, items[0] + "-${MACHINE}.ext4")
                    else:
                        bb.debug(1, "Set FLASHLAYOUT_PARTITION_BIN2LOAD_%s to %s." % (fl_label, items[0] + "-${DISTRO}-${MACHINE}.ext4"))
                        d.setVar('FLASHLAYOUT_PARTITION_BIN2LOAD_%s' % fl_label, items[0] + "-${DISTRO}-${MACHINE}.ext4")
                    if items[3]:
                        bb.debug(1, "Set FLASHLAYOUT_PARTITION_SIZE_%s to %s." % (fl_label, items[3]))
                        d.setVar('FLASHLAYOUT_PARTITION_SIZE_%s' % fl_label, items[3])
                    else:
                        bb.fatal('[PARTITIONS_CONFIG] Missing PARTITION_SIZE setting for % label' % fl_label)
                    if items[4]:
                        bb.debug(1, "Set FLASHLAYOUT_PARTITION_TYPE_%s to %s." % (fl_label, items[4]))
                        d.setVar('FLASHLAYOUT_PARTITION_TYPE_%s' % fl_label, items[4])
                        # Compute partition id according to type set
                        if items[4] == 'Binary':
                            part_id = '0x{0:0{1}X}'.format(id_bin, 2)
                            id_bin = id_bin + 1
                        else:
                            part_id = '0x{0:0{1}X}'.format(id_oth, 2)
                            id_oth = id_oth + 1
                        bb.debug(1, "Set FLASHLAYOUT_PARTITION_ID_%s to %s." % (fl_label, part_id))
                        d.setVar('FLASHLAYOUT_PARTITION_ID_%s' % fl_label, "%s" % part_id)
                    else:
                        bb.fatal('[PARTITIONS_CONFIG] Missing PARTITION_TYPE setting for % label' % fl_label)
                    break
}

# -----------------------------------------------------------------------------
# Manage specific var dependency:
# Because of local overrides within create_flashlayout_config() function, we
# need to make sure to add each variables to the vardeps list.

FLASHLAYOUT_LABELS_VARS = "CONFIG_LABELS PARTITION_LABELS TYPE_LABELS"
FLASHLAYOUT_LABELS_OVERRIDES = "${@' '.join('%s %s %s_%s' % (b, c, b, c) for b in d.getVar('FLASHLAYOUT_BOOTSCHEME_LABELS').split() for c in d.getVar('FLASHLAYOUT_CONFIG_LABELS').split())}"
do_create_flashlayout_config[vardeps] += "${@' '.join(['FLASHLAYOUT_%s_%s' % (v, o) for v in d.getVar('FLASHLAYOUT_LABELS_VARS').split() for o in d.getVar('FLASHLAYOUT_LABELS_OVERRIDES').split()])}"

FLASHLAYOUT_PARTITION_VARS = "ENABLE ID TYPE DEVICE OFFSET BIN2LOAD SIZE REPLACE_PATTERNS"
FLASHLAYOUT_PARTITION_CONFIGURED = "${@" ".join(map(lambda o: "%s" % d.getVar("FLASHLAYOUT_PARTITION_LABELS_%s" % o), d.getVar('FLASHLAYOUT_LABELS_OVERRIDES').split()))}"
FLASHLAYOUT_PARTITION_OVERRIDES = "${@' '.join('%s %s %s_%s' % (o, p, o, p) for o in d.getVar('FLASHLAYOUT_LABELS_OVERRIDES').split() for p in d.getVar('FLASHLAYOUT_PARTITION_CONFIGURED').split())}"
do_create_flashlayout_config[vardeps] += "${@' '.join(['FLASHLAYOUT_PARTITION_%s_%s' % (v, o) for v in d.getVar('FLASHLAYOUT_PARTITION_VARS').split() for o in d.getVar('FLASHLAYOUT_PARTITION_OVERRIDES').split()])}"

FLASHLAYOUT_DEVICE_VARS = "ALIGNMENT_SIZE BOARD_ENABLE START_OFFSET"
FLASHLAYOUT_PARTITION_DEVICE_CONFIGURED = "${@" ".join(map(lambda p: "%s" % d.getVar("DEVICE_%s" % p), d.getVar('DEVICE_STORAGE_NAMES').split()))}"
do_create_flashlayout_config[vardeps] += "${@' '.join(['DEVICE_%s_%s' % (v, o) for v in d.getVar('FLASHLAYOUT_DEVICE_VARS').split() for o in d.getVar('FLASHLAYOUT_PARTITION_DEVICE_CONFIGURED').split()])}"
