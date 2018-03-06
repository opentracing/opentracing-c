#!/bin/bash

# Copyright (c) 2018 Uber Technologies, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -e

function main() {
    local project_dir
    project_dir=$(git rev-parse --show-toplevel)
    cd "$project_dir" || exit 1

    local srcs
    srcs=$(git ls-files src |
           grep -E '\.c$' |
           grep -E -v '_test\.c$|protoc-gen')

    local skip_checks
    skip_checks="-clang-diagnostic-unused-command-line-argument,"
    skip_checks+="-clang-analyzer-security.insecureAPI.rand"

    local cmd
    for src in $srcs; do
        cmd="clang-tidy -p=build"
        cmd+=" -checks=\"${skip_checks}\""
        cmd+=" $src"
        echo "$cmd"
        eval "$cmd"
    done
}

main
