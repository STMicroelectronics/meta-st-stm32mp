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
            echo "[SDK ERROR] No 'export-user_ta' directory available in '<SDK_SYSROOT>/usr/include/optee'"
        fi
    fi
fi

export LIBGCC_LOCATE_CFLAGS=--sysroot=$SDKTARGETSYSROOT
