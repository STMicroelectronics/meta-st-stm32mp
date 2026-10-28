#inherit sbom-cve-check-common

do_stm32mp_sbom_compress() {
    extension="spdx.json sbom-cve-check.spdx.json sbom-cve-check.yocto.json"

    for ext in ${extension};
    do
        file=$(readlink  ${DEPLOY_DIR_IMAGE}/${IMAGE_LINK_NAME}.${ext})
        if [ -f ${DEPLOY_DIR_IMAGE}/$file ]; then
            name=$(basename $file)
            cd ${DEPLOY_DIR_IMAGE}/
            xz -z --keep $name
            ln -sf ${file}.xz ${IMAGE_LINK_NAME}.${ext}.xz
            cd -
        fi
    done
}
addtask do_stm32mp_sbom_compress after do_sbom_cve_check before do_build

SSTATETASKS += "do_stm32mp_sbom_compress"
do_stm32mp_sbom_compress[depends] += " xz-native:do_populate_sysroot"

python do_stm32mp_sbom_compress_setscene() {
    sstate_setscene(d)
}
addtask do_stm32mp_sbom_compress_setscene
