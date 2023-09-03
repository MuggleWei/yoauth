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
build_openssl=0

# directories
build_dir=$origin_dir/build
install_dir=$build_dir/usr
pkg_dir=$origin_dir/dist
dep_dir=$origin_dir/_deps

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

# create directories
if [ ! -d $build_dir ]; then
	echo "create build directory"
	mkdir -p $build_dir
fi

if [ ! -d $install_dir ]; then
	echo "create install directory"
	mkdir -p $install_dir
fi

if [ ! -d $pkg_dir ]; then
	echo "create package directory"
	mkdir -p $pkg_dir
fi

if [ ! -d $dep_dir ]; then
	echo "create dependency directory"
	mkdir -p $dep_dir
fi

# mugglec
echo "------------------------"
if [ $build_mugglec -eq 1 ]; then
	echo "build mugglec"
	cd $dep_dir

	mugglec_git=https://github.com/MuggleWei/mugglec.git
	mugglec_tag=v1.3.2
	mugglec_name=mugglec-$mugglec_tag
	mugglec_src_dir=$dep_dir/$mugglec_name
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
		-DBUILD_SHARED_LIBS=ON \
		-DMUGGLE_BUILD_STATIC_PIC=ON \
		-DMUGGLE_BUILD_TRACE=OFF \
		-DMUGGLE_BUILD_SANITIZER=OFF \
		-DBUILD_TESTING=OFF \
		-DMUGGLE_BUILD_EXAMPLE=OFF \
		-DMUGGLE_BUILD_BENCHMARK=OFF \
		-DCMAKE_INSTALL_PREFIX=$install_dir

	cmake --build $mugglec_build_dir
	cmake --build $mugglec_build_dir --target install
	#ln -sf $mugglec_build_dir/compile_commands.json $mugglec_src_dir/
else
	echo "option build mugglec off, ignore build mugglec"
fi
cd $origin_dir

# openssl
echo "------------------------"
if [ $build_openssl -eq 1 ]; then
	echo "build openssl"
	cd $dep_dir

	openssl_git=https://github.com/openssl/openssl.git
	openssl_tag=openssl-3.1.1
	openssl_name=openssl-$openssl_tag
	openssl_src_dir=$dep_dir/$openssl_name
	openssl_build_dir=$build_dir/_deps/openssl

	if [ ! -d $openssl_src_dir ]; then
		download_src $openssl_name $openssl_git $openssl_tag ${openssl_src_dir}_tmp
		mv ${openssl_src_dir}_tmp $openssl_src_dir
	fi

	if [ ! -d $openssl_build_dir ]; then
		mkdir -p $openssl_build_dir
	fi
	cd $openssl_build_dir

	$openssl_src_dir/configure \
		--prefix=$install_dir \
		--openssldir=$install_dir \
		threads \
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
	-DCMAKE_PREFIX_PATH=$install_dir \
	-DCMAKE_INSTALL_PREFIX=$install_dir
#ln -sf $build_dir/compile_commands.json $origin_dir/
cmake --build $build_dir
cmake --build $build_dir --target install

# package
cd $install_dir
tar -czvf yoauth.tar.gz bin lib/*.so*
mv yoauth.tar.gz $pkg_dir
