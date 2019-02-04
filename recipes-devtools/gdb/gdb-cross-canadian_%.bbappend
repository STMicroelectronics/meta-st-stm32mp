FILESEXTRAPATHS_prepend_stm32mpcommon := "${THISDIR}/files:"

#Add scripts for gdb/openocd/eclipse
SRC_URI_append_stm32mpcommon += " \
    file://gdbinit \
"

# Ref to yocto default conf at:
# openembedded-core/meta/recipes-devtools/gdb/gdb-common.inc
# We force activation of tui for the "layout" command.
EXTRA_OECONF_append_stm32mpcommon = " \
    --enable-tui \
"

do_install_append_stm32mpcommon() {
   install -d ${D}/${bindir}/
   cp -a ${WORKDIR}/gdbinit ${D}/${bindir}/
}

