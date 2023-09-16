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

### Kas Support

If you are familiar with `kas` tool, you can use it to setup this layer only or other custom layer that includes this layer's `kas` configuration.

* To build images provided by this layer (`stm32mp1` machine is supported):

```sh
git clone https://github.com/STMicroelectronics/meta-st-stm32mp -b <branch>
kas build meta-st-stm32mp/kas/kas-st-stm32mp1-##image##.yml
```

for `##image##` is one of: `bootfs`, `userfs` or `vendorfs`.

* To use `kas` from another custom layer that uses this layer, `kas` supports including a file from another layer that is not the one containing your `kas` files.

**Example**:

* `meta-custom/kas-custom-image.yml`

```yaml
head:
  version: 5
  includes:
    - repo: meta-st-stm32mp
      file: kas/include/kas-st.yml

repos:
  meta-st-stm32mp:
    url: https://github.com/STMicroelectronics/meta-st-stm32mp
    path: layers
    refspec: kirkstone

target: custom-image
machine: stm32mp1
```

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
