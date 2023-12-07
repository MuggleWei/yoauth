#!/bin/bash

# handle argv
if [ "$#" -lt 1 ]; then
	echo "[WARNING] Usage: bootstrap.sh <Debug|Release|RelWithDebInfo>"
	echo "[WARNING] bootstrap without build type, use release by default"
	BUILD_TYPE=release
else
	# to lowercase
	BUILD_TYPE=$(echo $1 | tr '[:upper:]' '[:lower:]')
fi

# project root directory
origin_dir="$(dirname "$(readlink -f "$0")")"

# build option
build_mugglec=1
build_openssl=1

# directories
build_dir=$origin_dir/build
deps_dir=$origin_dir/_deps
dist_dir=$build_dir/dist
pkg_dir=$origin_dir/dist

abi=arm64-v8a
min_sdk=29

# function
download_src() {
	local name=$1
	local git_repo=$2
	local tag=$3
	local src_dir=$4

	if [ -d $src_dir ]; then
		echo "$name source directory already exists"
	else
		echo "$name source directory not exists, clone"
		echo "name=$name"
		echo "git_repo=$git_repo"
		echo "tag=$tag"
		echo "src_dir=$src_dir"

		git clone --branch $tag --depth 1 $git_repo $src_dir
	fi
}

# check ndk
if [ -z "$ANDROID_NDK_ROOT" ]; then
	echo "run without ANDROID_NDK_ROOT"
	exit 1
else
	echo "ndk: $ANDROID_NDK_ROOT"
fi

# clean
if [ ! -d $build_dir ]; then
	echo "create build directory"
	mkdir -p $build_dir
fi

if [ ! -d $deps_dir ]; then
	echo "create dependency directory"
	mkdir -p $deps_dir
fi

if [ ! -d $dist_dir ]; then
	echo "create install directory"
	mkdir -p $dist_dir
fi

if [ ! -d $pkg_dir ]; then
	echo "create package directory"
	mkdir -p $pkg_dir
fi

# mugglec
echo "------------------------"
if [ $build_mugglec -eq 1 ]; then
	echo "build mugglec"
	cd $deps_dir

	mugglec_git=https://github.com/MuggleWei/mugglec.git
	mugglec_tag=v1.4.4
	mugglec_name=mugglec-$mugglec_tag
	mugglec_src_dir=$deps_dir/$mugglec_name
	mugglec_build_dir=$build_dir/_deps/mugglec

	if [ ! -d $mugglec_src_dir ]; then
		download_src $mugglec_name $mugglec_git $mugglec_tag ${mugglec_src_dir}_tmp
		mv ${mugglec_src_dir}_tmp $mugglec_src_dir
	fi

	if [ ! -d $mugglec_build_dir ]; then
		mkdir -p $mugglec_build_dir
	fi
	cd $mugglec_build_dir

	cmake \
		-S $mugglec_src_dir -B $mugglec_build_dir \
		-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
		-DBUILD_SHARED_LIBS=OFF \
		-DCMAKE_INSTALL_PREFIX=$dist_dir \
		-DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_ROOT/build/cmake/android.toolchain.cmake \
		-DANDROID_ABI=$abi \
		-DANDROID_PLATFORM=android-$min_sdk
	cmake --build $mugglec_build_dir
	cmake --build $mugglec_build_dir --target install
else
	echo "option build mugglec off, ignore build mugglec"
fi
cd $origin_dir

# openssl
echo "------------------------"
if [ $build_openssl -eq 1 ]; then
	echo "build openssl"
	cd $deps_dir

	openssl_git=https://github.com/openssl/openssl.git
	openssl_tag=openssl-3.1.4
	openssl_name=openssl-$openssl_tag
	openssl_src_dir=$deps_dir/$openssl_name
	openssl_build_dir=$build_dir/_deps/openssl

	if [ ! -d $openssl_src_dir ]; then
		download_src $openssl_name $openssl_git $openssl_tag ${openssl_src_dir}_tmp
		mv ${openssl_src_dir}_tmp $openssl_src_dir
	fi

	if [ ! -d $openssl_build_dir ]; then
		mkdir -p $openssl_build_dir
	fi
	cd $openssl_build_dir

	PATH=$ANDROID_NDK_ROOT/toolchains/llvm/prebuilt/linux-x86_64/bin:$PATH
	$openssl_src_dir/Configure \
		android-arm64 \
		-U__ANDROID_API__ \
		-D__ANDROID_API__=$min_sdk \
		--prefix=$dist_dir \
		--openssldir=$dist_dir \
		--libdir=lib \
		threads \
		no-shared \
		no-comp \
		-Wl,-rpath,\\\$\$ORIGIN/../lib \
		no-tests \
		--$BUILD_TYPE

	#bear -- make
	make
	make install_sw
	#ln -sf $openssl_build_dir/compile_commands.json $openssl_src_dir
else
	echo "option build openssl off, ignore build openssl"
fi
cd $origin_dir

# build
cd $origin_dir
cmake \
	-S $origin_dir -B $build_dir \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DBUILD_SHARED_LIBS=OFF \
	-DCMAKE_PREFIX_PATH=$dist_dir \
	-DCMAKE_INSTALL_PREFIX=$dist_dir \
	-DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_ROOT/build/cmake/android.toolchain.cmake \
	-DANDROID_ABI=$abi \
	-DANDROID_PLATFORM=android-$min_sdk
cmake --build $build_dir
cmake --build $build_dir --target install

# package
cd $dist_dir
tar -czvf yoauth.tar.gz bin/yoauth*
mv yoauth.tar.gz $pkg_dir
