#!/bin/bash

origin_dir="$(dirname "$(readlink -f "$0")")"

build_dir=$origin_dir/build
pkg_dir=$origin_dir/dist

rm -rf $build_dir
rm -rf $pkg_dir
