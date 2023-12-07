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
dist_dir=$build_dir/usr
pkg_dir=$origin_dir/dist

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
		-DBUILD_SHARED_LIBS=ON \
		-DMUGGLE_BUILD_STATIC_PIC=ON \
		-DMUGGLE_BUILD_TRACE=OFF \
		-DMUGGLE_BUILD_SANITIZER=OFF \
		-DBUILD_TESTING=OFF \
		-DMUGGLE_BUILD_EXAMPLE=OFF \
		-DMUGGLE_BUILD_BENCHMARK=OFF \
		-DCMAKE_INSTALL_PREFIX=$dist_dir

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

	$openssl_src_dir/Configure \
		--prefix=$dist_dir \
		--openssldir=$dist_dir \
		--libdir=lib \
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
	-DCMAKE_PREFIX_PATH=$dist_dir \
	-DCMAKE_INSTALL_PREFIX=$dist_dir
#ln -sf $build_dir/compile_commands.json $origin_dir/
cmake --build $build_dir
cmake --build $build_dir --target install

# package
cd $origin_dir

ver=$(head -1 version.txt)
uname_s="$(uname -s)"
case "${uname_s}" in
	Linux*) sys_name=linux;;
	Darwin*) sys_name=mac;;
	*) sys_name=unknown
esac
glibc_ver=$(ldd --version | awk '/ldd/ {print $NF}')
arch=$(uname -m)

pkg_name=yoauth-v$ver-$sys_name-glibc$glibc_ver-$arch

cd $dist_dir
if [[ "$OSTYPE" == "darwin"* ]]; then
	# force change rpath to openssl in yoauth
	if [ $build_openssl -eq 1 ]; then
		install_name_tool -change "$dist_dir/lib/libcrypto.3.dylib" @rpath/libcrypto.3.dylib bin/yoauth
	fi
	tar -czvf ${pkg_name}.tar.gz bin/yoauth* lib/*.dylib*
else
	tar -czvf ${pkg_name}.tar.gz bin/yoauth* lib/*.so*
fi
mv ${pkg_name}.tar.gz $pkg_dir
