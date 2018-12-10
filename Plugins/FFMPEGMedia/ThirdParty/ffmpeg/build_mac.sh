#!/bin/sh

# install yasm
# http://macappstore.org/yasm/

# install pkg-config
# http://macappstore.org/pkg-config/

PREFIX=$(pwd)/build_result/

./configure \
    --target-os=darwin \
    --prefix=${PREFIX} \ 
    --extra-cxxflags="-mmacosx-version-min=10.11"

make clean
make -j4
make install