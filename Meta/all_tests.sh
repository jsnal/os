#!/bin/sh

. ./Meta/env.sh && \
    make -C Build/Tests install && \
    find Tests/bin -maxdepth 1 -type f -exec '{}' -v \;
