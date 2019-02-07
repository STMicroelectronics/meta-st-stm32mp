# Summary

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
URI: git://github.com/openembedded/oe-core.git
layers: meta
branch: same dedicated branch as meta-st-stm32mp
revision: HEAD
```

```
URI: git://github.com/openembedded/meta-openembedded.git
layers: meta-openembedded/meta-python meta-openembedded/meta-oe
branch: same dedicated branch as meta-st-stm32mp
revision: HEAD
```

