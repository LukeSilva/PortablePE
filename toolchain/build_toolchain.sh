#!/bin/bash

export PREFIX="$PWD/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

echo "Need to get new versions of toolchain, dont use me"
echo "Fails with: gcc-6.3.0/gcc/ubsan.c:1474:23: error: ISO C++ forbids comparison between pointer and integer [-fpermissive]"
exit 1

echo "Removing old build directories"
rm -rf build-binutils || true
rm -rf build-gcc || true
rm -rf binutils-2.28 || true
rm -rf gcc-6.3.0 || true
rm -rf cross | true


echo "Extracting source code"
tar -xf binutils-2.28.tar.bz2
tar -xf gcc-6.3.0.tar.bz2

echo "Creating prefix directory"
mkdir "$PREFIX"

echo "Building binutils"
mkdir build-binutils
cd build-binutils
../binutils-2.28/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror || exit 1
make || exit 2
echo "Installing binutils"
make install || exit 3

cd ..

echo "Building gcc"
mkdir build-gcc
cd build-gcc
../gcc-6.3.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c --without-headers || exit 4
make all-gcc || exit 5
make all-target-libgcc || exit 6

echo "Installing gcc"
make install-gcc || exit 7
make install-target-libgcc || exit 8

cd ..
