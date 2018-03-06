#!/bin/bash

set -e

function usage() {
    echo "usage: ${BASH_SOURCE} <prefix>" 1>&2
}

function main() {
    local prefix
    prefix="${BASH_ARGV[0]}"

    export PKG_CONFIG_PATH="$prefix/lib/pkgconfig:$PKG_CONFIG_PATH"

    if [ "x$prefix" = "x" ]; then
        usage
        exit 1
    fi

    mkdir -p "$prefix"

    if [ ! -f "$prefix/bin/cmake" ]; then
        if [ ! -f "cmake-3.10.0-rc5-Linux-x86_64.sh" ]; then
            curl -L -O "https://cmake.org/files/v3.10/cmake-3.10.0-rc5-Linux-x86_64.sh"
        fi
        bash cmake-3.10.0-rc5-Linux-x86_64.sh --skip-license --prefix="$prefix"
    fi
}

main
