# Burstcoin plotting tools

The set of tools that contains everything you need to plot your hard drives for Burstcoin mining in easy and comfortable way.
## Currently implemented tools:
1. Optimizer

2. Nonce overlap checker - an easy way to check if any space is wasted

3. Plot planner - automated plotting appliance that searches for existing plot files, calculates a set of free nonces and plots space in folders you want to. Works well on SMR drives (unlike omdcct/cg_obup) by using a temporary folder on a normal hard drive (but you don't have to of course). Since plotter is not implemented yet, cryo's GPU plotter is needed.

## Planned:
1. OpenCL Plotter

## Project features:
1. High quality code allows easy modification and many features

2. Automated testing

3. Unit tests gives confidence that your data are correct

## Planned:
1. Resumability - plotting/optimizing can be interrupted in the middle and then resumed

2. Automation

| Travis status | [![Build Status](https://travis-ci.org/Kristian-Popov/Burstcoin-OpenCL-plotter.svg?branch=master)](https://travis-ci.org/Kristian-Popov/Burstcoin-OpenCL-plotter) |
| --- | --- |

## Limitations

Currently the only supported platform is Linux x64 (that is 64-bit), tested only on Ubuntu 16.04 Xenial Xerus (although unit tests on Travis environment are run on Ubuntu 14.04 Trusty Tahr).

## Prerequisites

Very recent versions of Boost and CMake are needed for build. New CMake can be downloaded to a subfolder `cmake` at your home directory by using these simple instructions

```
pushd ~
CMAKE_URL="https://cmake.org/files/v3.10/cmake-3.10.2-Linux-x86_64.tar.gz";
mkdir cmake && wget --quiet -O - ${CMAKE_URL} | tar --strip-components=1 -xz -C cmake;
export PATH=~/cmake/bin:${PATH};
popd
```
These commands download recent CMake, unpack it in subfolder `cmake` and change system PATH variable (temporarily) so that CMake is accessed first.

## Compilation process

At first please clone the repository:

```
git clone https://github.com/Kristian-Popov/Burstcoin-OpenCL-plotter.git
cd Burstcoin-OpenCL-plotter
git submodule update --init --recursive
```

A special script is used to keep track of current version, it transparently integrates with Git via hooks, but you have to set up them first and update version for the first time manually:

```
./setup-hooks.sh
scripts/update-version.sh
```

Now a recent Boost should be downloaded and unpacked:

```
wget -O boost_1_65_1.tar.gz https://dl.bintray.com/boostorg/release/1.65.1/source/boost_1_65_1.tar.gz
tar xzf boost_1_65_1.tar.gz
cd boost_1_65_1/
```

Time to build Boost:

```
./bootstrap.sh --with-libraries=program_options,filesystem,system,log
./b2 -d0 link=static threading=multi address-model=64
```

Finally time to build Burstcoin plotting tools and unit tests.

```
mkdir -p build
cd build
cmake -DBOOST_ROOT="../boost_1_65_1" ..
make
```


## Running the tests

Just call this command from a build directory

```
./unit_tests/unit_tests
```

## Continuous integration

Automated testing is done on Travis platform. If build is successful, that means that:
1. Code can be compiled
2. All unit tests are passed
3. Output files for optimizer are identical to ones created by omdcct

The last step is the most important to guarantee data integrity of output files. Verification is performed by creating real plot source files, performing neccesary operations and byte-to-byte comparison to files created by widely used plotter omdcct. This procedure makes sure you'll get the same data as you would with other widely used plotters.

Currently the whole test suite is run on code compiled by two compilers: g++ version 5 (close to default version on Ubuntu 16.04 Xenial Xerus I personally prefer) and g++ version 7 (very modern stable release).
Ubuntu 14.04 Trusty Tahr is used as operating system.

## Built With

This project is implemented in C++14 and uses the following third-party libraries:
1. Boost (Log, Filesystem etc.)
2. Catch

## Donations are appreciated

Implemented by Kristian Popov (https://github.com/Kristian-Popov/)

Donations will allow me to spend more time on this project, provide more quality code, cool features and better user support.
BURST: BURST-K8Y4-9ZB4-97SV-6CH4T

## License

This project is licensed under the Apache License, Version 2.0 - see the [LICENSE.md](https://github.com/Kristian-Popov/Burstcoin-OpenCL-plotter/blob/master/LICENSE.md) file for details

## Acknowledgments

* Hat tip to third-party library developers for their great code and high-quality work
* My family for support and cookies
* Inspiration
