#!/bin/sh

. ./meta/env.sh && \
    make -C libc && \
    make -C Kernel && \
    ./meta/build_grub.sh && \
    ./meta/run.sh $@
