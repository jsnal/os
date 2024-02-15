#!/usr/bin/env bash

set -eo pipefail

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

ARCH="i686"
PREFIX="$DIR/local"
TARGET="$ARCH-pc-os"
PATH="$PREFIX/bin:$PATH"

if [ ! -d "binutils" ] || [ ! -d "gcc" ]; then
    echo "Submodules have not been cloned"
fi

pushd "binutils"
    echo "Automaking LD"
    pushd ld
        autoreconf
        automake
    popd

    if [ ! -d "build" ]; then
        mkdir build
        pushd "build"
        echo "Configuring binutils"
        ../configure \
            --target=$TARGET \
            --prefix="$PREFIX" \
            --with-sysroot \
            --disable-nls \
            --disable-werror

        echo "Building binutils"
        make all -j4

        echo "Installing binutils"
        make install
        popd
    fi

popd

pushd "gcc"
  echo "Downloading dependencies"
  ./contrib/download_prerequisites

  echo "Automaking libstd++-v3"
  pushd libstdc++-v3
    autoconf
  popd

  if [ ! -d "build" ]; then
      mkdir build
  fi

  pushd "build"
      echo "Configuring gcc"
      ../configure --target=$TARGET \
          --prefix="$PREFIX" \
          --disable-nls \
          --without-headers \
          --enable-languages=c,c++

      echo "Building gcc"
      make all-gcc -j4
      make all-target-libgcc -j4

      echo "Installing gcc"
      make install-gcc -j4
      make install-target-libgcc -j4
  popd
popd
