#!/bin/sh

# install yasm
# http://macappstore.org/yasm/

# install pkg-config
# http://macappstore.org/pkg-config/

./configure \
    --target-os=mac \
    --extra-cxxflags="-mmacosx-version-min=10.10"

make clean
make -j4
make install