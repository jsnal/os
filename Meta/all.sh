#!/bin/sh


. ./Meta/env.sh && \
    make -C Build Kernel && \
    make -C Build c && \
    cp -fv $DIR/Build/Kernel/Kernel $DIR/Kernel && \
    cp -fv $DIR/Build/LibC/libc.a $DIR/LibC/libc.a && \
    ./Meta/build_grub.sh && \
    ./Meta/run.sh $@
