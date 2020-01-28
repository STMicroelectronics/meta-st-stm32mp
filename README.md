## Summary

**meta-st-stm32mp BSP layer** is a layer containing the STMicroelectronics bsp metadata for current versions
of stm32mp.

This layer relies on OpenEmbedded/Yocto build system that is provided through
Bitbake and OpenEmbedded-Core layers or Poky layer all part of the Yocto Project

The Yocto Project has extensive documentation about OE including a reference manual
which can be found at:

 * **http://yoctoproject.org/documentation**

For information about OpenEmbedded, see the OpenEmbedded website:

 * **http://www.openembedded.org/**

This layer depends on:

```
[OECORE]
URI: https://github.com/openembedded/openembedded-core.git
layers: meta
branch: same dedicated branch as meta-st-stm32mp
revision: HEAD
[BITBAKE]
URI: https://github.com/openembedded/bitbake.git
branch: branch associated to oecore branch
revision: HEAD
```
or
```
[OECORE]
URI: git://git.yoctoproject.org/poky
layers: meta
branch: same dedicated branch as meta-st-stm32mp
revision: HEAD
```

```
[META-OPENEMBEDDED]
URI: git://github.com/openembedded/meta-openembedded.git
layers: meta-python meta-oe
branch: same dedicated branch as meta-st-stm32mp
revision: HEAD
```

The dependency (meta-python) are due to the usage of OPTEE which require to use some python packages.

## EULA

Some SoC depends on firmware and/or packages that are covered by
 STMicroelectronics EULA. To have the right to use those binaries in your images you need to read and accept the EULA available as:

conf/eula/$MACHINE, e.g. conf/eula/stm32mp1

In order to accept it, you should add, in your local.conf file:

ACCEPT_EULA_$MACHINE = "1", e.g.: ACCEPT_EULA_stm32mp1 = "1"

If you do not accept the EULA the generated image will be missing some
components and features.

## Contributing
If you want to contribute changes, you can send Github pull requests at
**https://github.com/stmicroelectronics/meta-st-stm32mp/pulls**.


## Maintainers
 - Christophe Priouzeau <christophe.priouzeau@st.com>
 - Sebastien Gandon <sebastien.gandon@st.com>
 - Bernard Puel <bernard.puel@st.com>
