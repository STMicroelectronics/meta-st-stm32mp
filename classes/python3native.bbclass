inherit python3-dir

PYTHON="${STAGING_BINDIR_NATIVE}/python3-native/python3"
EXTRANATIVEPATH += "python3-native"
DEPENDS_append = " python3-native "

# python-config and other scripts are using distutils modules
# which we patch to access these variables
export STAGING_INCDIR
export STAGING_LIBDIR

# Packages can use
# find_package(PythonInterp REQUIRED)
# find_package(PythonLibs REQUIRED)
# which ends up using libs/includes from build host
# Therefore pre-empt that effort
export PYTHON_LIBRARY="${STAGING_LIBDIR}/lib${PYTHON_DIR}${PYTHON_ABI}.so"
export PYTHON_INCLUDE_DIR="${STAGING_INCDIR}/${PYTHON_DIR}${PYTHON_ABI}"

export _PYTHON_SYSCONFIGDATA_NAME="_sysconfigdata"
# Unset these to stop python trying to report the target Python setup
python () {
    if bb.data.inherits_class('devtool-source', d):
        bb.warn("_PYTHON_SYSCONFIGDATA_NAME unexported for devtool to fix an issue with Python on Ubuntu 20.04. Should be fixed in later version of bitbake")
        d.setVarFlag('_PYTHON_SYSCONFIGDATA_NAME', 'unexport', '1')
}

# suppress host user's site-packages dirs.
export PYTHONNOUSERSITE = "1"

# autoconf macros will use their internal default preference otherwise
export PYTHON
