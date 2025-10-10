export TEEC_EXPORT=$SDKTARGETSYSROOT/usr
if [ -d "$SDKTARGETSYSROOT/usr/include/optee/export-user_ta" ]; then
    export TA_DEV_KIT_DIR=$SDKTARGETSYSROOT/usr/include/optee/export-user_ta
else
    if [ -d "$SDKTARGETSYSROOT/usr/include/optee/export-user_ta_arm32" ]; then
        export TA_DEV_KIT_DIR=$SDKTARGETSYSROOT/usr/include/optee/export-user_ta_arm32
    else
        if [ -d "$SDKTARGETSYSROOT/usr/include/optee/export-user_ta_arm64" ]; then
            export TA_DEV_KIT_DIR=$SDKTARGETSYSROOT/usr/include/optee/export-user_ta_arm64
        else
            echo "[SDK ERROR] Issue to optee export-user_ta directory"
        fi
    fi
fi

export LIBGCC_LOCATE_CFLAGS=--sysroot=$SDKTARGETSYSROOT
