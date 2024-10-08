# An ARMv8 Virtual Platform (AVP64)

[![cmake](https://github.com/aut0/avp64/actions/workflows/cmake.yml/badge.svg?event=push)](https://github.com/aut0/avp64/actions/workflows/cmake.yml)
[![nightly](https://github.com/aut0/avp64/actions/workflows/nightly.yml/badge.svg?event=schedule)](https://github.com/aut0/avp64/actions/workflows/nightly.yml)
[![lint](https://github.com/aut0/avp64/actions/workflows/lint.yml/badge.svg?event=push)](https://github.com/aut0/avp64/actions/workflows/lint.yml)
[![style](https://github.com/aut0/avp64/actions/workflows/style.yml/badge.svg?event=push)](https://github.com/aut0/avp64/actions/workflows/style.yml)

This repository contains an ARMv8 multicore virtual platform.
It was built at the [Institute for Communication Technologies and Embedded Systems at RTWH Aachen University](https://www.ice.rwth-aachen.de/).
The following target software configurations were tested (see [avp64-sw](https://github.com/aut0/avp64_sw)):

- CoreMark
- Dhrystone
- Whetstone
- STREAM
- Linux single-, dual-, quad-, octa-core
- Xen single- and dual-core

----

## Build & Installation

1. Clone git repository including submodules:

    ```bash
    git clone --recursive https://github.com/aut0/avp64
    ```

1. Chose directories for building and deployment:

    ```text
    <source-dir>  location of your repo copy,     e.g. /home/lukas/avp64
    <build-dir>   location to store object files, e.g. /home/lukas/avp64/BUILD
    <install-dir> output directory for binaries,  e.g. /opt/avp64
    ```

1. Configure and build the project using `cmake`. During configuration you must
   state whether or not to build the runner (vp executable) and the unit tests:

     - `-DAVP64_BUILD_RUNNER=[ON|OFF]`: build runner (default: `ON`)
     - `-DAVP64_BUILD_TESTS=[ON|OFF]`: build unit tests (default: `OFF`)

   Release and debug build configurations are controlled via the regular cmake parameters:

   ```bash
   mkdir -p <build-dir>
   cd <build-dir>
   cmake -DCMAKE_INSTALL_PREFIX=<install-dir> -DCMAKE_BUILD_TYPE=RELEASE <source-dir>
   make -j `nproc`
   sudo make install
   ```

   If building with `-DAVP64_BUILD_TESTS=ON` you can run all unit tests using `make test` within `<build-dir>`.

1. After installation, the following new files should be present:

    ```bash
    <install-dir>/bin/avp64-runner        # executable program
    <install-dir>/lib/libocx-qemu-arm.so
    ```

1. If the library `libocx-qemu-arm.so` cannot be found, add the lib folder to `LD_LIBRARY_PATH`:

    ```bash
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<install-dir>/lib/
    ```

----

## Run

To run the platform, a configuration and the corresponding target software files are required.
Examples can be found in the [avp64-sw](https://github.com/aut0/avp64_sw) repository.
The configuration files and target software can be placed in the [sw](sw/) folder to be automatically copied to the install directory on `make install`.
To fetch the latest buildroot Linux build, the [utils/fetch_buildroot_linux](./utils/fetch_buildroot_linux) script can be used.
This script downloads the latest prebuilt linux image and configuration files and places them in the [sw](sw/) folder.  

Run the platform using a config file from the [sw](sw/) folder:

```bash
<install-dir>/bin/avp64-runner -f <install-dir>/sw/<config-file>
```

For more details on run parameters please look [here](https://github.com/machineware-gmbh/vcml).  

To stop the platform, press <kbd>Ctrl</kbd> + <kbd>a</kbd> + <kbd>x</kbd> .

----

## Maintaining Multiple Builds

Debug builds (i.e. `-DCMAKE_BUILD_TYPE=DEBUG`) are intended for developers
that like to make changes on `avp64` and want to track down bugs.
Note that these builds operate significantly slower than optimized release
builds and should therefore not be used for VPs that are used productively,
e.g. for target software development.
To maintain both builds from a single source repository, try the following:

```bash
git clone --recurse-submodules https://github.com/aut0/avp64 && cd avp64

home=$PWD
for type in "DEBUG" "RELEASE"; do
    install="$home/BUILD/$type"
    build="$home/BUILD/$type/BUILD"
    mkdir -p $build && cd $build
    cmake -DCMAKE_BUILD_TYPE=$type -DCMAKE_INSTALL_PREFIX=$install $home
    make install
done
```

Afterward, you can find the builds in:

```bash
<source-dir>/avp64/BUILD/DEBUG    # for the debug build or
<source-dir>/avp64/BUILD/RELEASE  # for the release build
```

----

## Tutorial

A baisc tutorial that shows how to debug the executed target software using [Visual Studio Code](https://code.visualstudio.com/) can be found in the [vscode-tutorial](./vscode-tutorial/) folder.
Run the [vscode-tutorial/setup.bash](./vscode-tutorial/setup.bash) script to download the Linux image and Linux Kernel Source files and setup Visual Studio Code.
See the corresponding [Readme](./vscode-tutorial/README.md) for further details.

----

## Documentation

The VCML documentation can be found
[here](https://github.com/machineware-gmbh/vcml).

----

## License

This project is licensed under the MIT license - see the
[LICENSE](LICENSE) file for details.
