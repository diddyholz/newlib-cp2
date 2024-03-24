# newlib-cp2
A newlib port for the Classpad II (fx-CP400) to be used alongside the [hollyhock SDK](https://github.com/SnailMath/hollyhock-2/).
## Prerequisites
- An SH4 cross-compiler (preferably gcc)
  - See: https://github.com/SnailMath/hollyhock-2/blob/master/doc/user/patching.md
## Building
### Environment variables
First, set an environment variable `$PREFIX` that holds the path to the directory where newlib should be installed to. I would advise to install it next to your hollyhock SDK.

Next, set an environment variable `$TARGET` to the value `sh4-elf`.

Example:

```sh
export PREFIX="$SDK_DIR/newlib"
export TARGET="sh4-elf"
``` 

### Preparing the source tree
Clone this repository using the following command: 

```sh
git clone https://github.com/diddyholz/newlib-cp2
```

Change directory into the cloned repository and create a directory named `build`. Cd into this directory.
```sh
cd newlib-cp
mkdir build
cd build
```

### Building and installing
To finally build newlib, first configure the build directory as follows:
```sh
../configure --target=$TARGET --prefix=$PREFIX
```

Now run make to build and install newlib into your prefix directory.
```sh
make all
make install
```
## Utilizing newlib in your hollyhock project
To utilize newlib with hollyhock, a template [linker file](templates/linker-template.ld) and [Makefile](templates/Makefile-Template) are supplied in this repository.
