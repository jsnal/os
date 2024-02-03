#!/bin/sh

. ./meta/env.sh && \
    make -C libc && \
    make -C kernel && \
    ./meta/build_grub.sh && \
    ./meta/run.sh $@
