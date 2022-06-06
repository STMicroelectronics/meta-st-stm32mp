do_configure:prepend:stm32mpcommon() {
    PLUGIN_PATH=$(${CC} -print-file-name=plugin)
    if [ -e "${PLUGIN_PATH}/include/plugin-version.h" ]; then
        bbnote "Will remove ${PLUGIN_PATH}/include/plugin-version.h"
        rm -f ${PLUGIN_PATH}/include/plugin-version.h
    fi
}

