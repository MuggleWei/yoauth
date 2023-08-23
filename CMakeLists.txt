cmake_minimum_required (VERSION 3.18.6)
project(yoauth)

################################
# general config
################################

message("-- use c compiler ${CMAKE_C_COMPILER}")
message("-- use c++ compiler ${CMAKE_CXX_COMPILER}")

# set compile parameter
if (${CMAKE_C_COMPILER_ID} STREQUAL GNU)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra")
elseif (${CMAKE_C_COMPILER_ID} MATCHES Clang)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -Wall -Wextra -Wno-missing-field-initializers")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -Wno-missing-field-initializers")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE)
endif()

# set standard and print features
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

message("-- c compiler support features: ")
foreach(feature ${CMAKE_C_COMPILE_FEATURES})
	message("support feature: ${feature}")
endforeach()

# for vim plugin - YCM
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# set use folder in vs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################
# options
################################

option(BUILD_SHARED_LIBS "Build shared or static library" ON)
option(BUILD_TESTING "Build unittest" OFF)
set(CMAKE_INSTALL_LIBDIR lib)

# build flags
message("# BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
message("# BUILD_TESTING: ${BUILD_TESTING}")

################################
# deps
################################

message("------------------------------")
message("# find dependencies begin")
foreach(path ${CMAKE_PREFIX_PATH})
	message("# CMake Prefix Path: ${path}")
endforeach()

find_package(mugglec REQUIRED)
if (mugglec_FOUND)
	message("deps mugglec version: ${mugglec_VERSION}")
	message("deps mugglec dir: ${mugglec_DIR}")
else()
	message(FATAL_ERROR "failed found mugglec")
endif()

find_package(OpenSSL)
if (OPENSSL_FOUND)
	message("deps openssl version: ${OPENSSL_VERSION}")
	message("deps openssl include dir: ${OPENSSL_INCLUDE_DIR}")
	message("deps openssl crypto lib: ${OPENSSL_CRYPTO_LIBRARY}")
	message("deps openssl ssl lib: ${OPENSSL_SSL_LIBRARY}")
else()
	message(FATAL_ERROR "Can't find openssl")
endif()

message("# find dependencies end")
message("------------------------------")

################################
# yoauth
################################

# version
file(STRINGS "version.txt" yoauth_version)
string(REPLACE "-" ";" yoauth_semver_ext ${yoauth_version})
list(GET yoauth_semver_ext 0 yoauth_semver)
string(REPLACE "." ";" yoauth_semver_list ${yoauth_semver})

list(GET yoauth_semver_list 0 YOAUTH_VERSION_MAJOR)
list(GET yoauth_semver_list 1 YOAUTH_VERSION_MINOR)
list(GET yoauth_semver_list 2 YOAUTH_VERSION_PATCH)
set(YOAUTH_VERSION "${YOAUTH_VERSION_MAJOR}.${YOAUTH_VERSION_MINOR}.${YOAUTH_VERSION_PATCH}")

# configure
set(yoauth_src_dir ${CMAKE_CURRENT_LIST_DIR}/src)
set(yoauth_gen_dir ${CMAKE_CURRENT_BINARY_DIR}/generated)

if (BUILD_SHARED_LIBS)
	set(YOAUTH_USE_DLL ON)
	set(YOAUTH_LIB_TYPE SHARED)
else()
	set(YOAUTH_USE_DLL OFF)
	set(YOAUTH_LIB_TYPE STATIC)
endif()

include(CheckIncludeFile)
check_include_file(threads.h YOAUTH_HAVE_THREADS)

configure_file(
	"${yoauth_src_dir}/yoauth/config.h.in"
	"${yoauth_gen_dir}/yoauth/config.h")
FILE(GLOB_RECURSE yoauth_gen_h "${yoauth_gen_dir}/yoauth/*.h")

# lib
FILE(GLOB_RECURSE yoauth_src "${yoauth_src_dir}/yoauth/*.c")
FILE(GLOB_RECURSE yoauth_h "${yoauth_src_dir}/yoauth/*.h")
LIST(REMOVE_ITEM yoauth_src "${yoauth_src_dir}/yoauth/main.c")
add_library(yoauth ${YOAUTH_LIB_TYPE}
	${yoauth_src}
	${yoauth_h}
	${yoauth_gen_h})
target_include_directories(yoauth PUBLIC
	$<BUILD_INTERFACE:${yoauth_src_dir}>
	$<BUILD_INTERFACE:${yoauth_gen_dir}>
	$<INSTALL_INTERFACE:include>)
target_link_libraries(yoauth mugglec "${OPENSSL_CRYPTO_LIBRARY}")
if (UNIX)
	target_link_libraries(yoauth m)
endif()
target_compile_definitions(yoauth
	PRIVATE YOAUTH_EXPORTS MUGGLE_HOLD_LOG_MACRO)
set_target_properties(yoauth PROPERTIES
	DEBUG_POSTFIX d
	VERSION ${YOAUTH_VERSION}
	SOVERSION ${YOAUTH_VERSION_MAJOR})

# win debug
if (WIN32)
	foreach(subdir ${CMAKE_PREFIX_PATH})
		file(GLOB dll_files "${subdir}/bin/*.dll")
		foreach(src_file ${dll_files})
			file(RELATIVE_PATH rel_path ${src_file} ${subdir}/bin)
			get_filename_component(dst_name ${src_file} NAME)
			set(dst_file "$(OutDir)${dst_name}")
			add_custom_command(
				TARGET yoauth
				POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different "${src_file}" ${dst_file}
				DEPENDS "${src_file}"
				COMMENT "sync ${src_file} to ${dst_file}"
			)
		endforeach(src_file ${dll_files})
	endforeach(subdir ${CMAKE_PREFIX_PATH})
endif()

################################
# install
################################
include(GNUInstallDirs)
install(TARGETS yoauth
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")
if (WIN32)
	foreach(subdir ${CMAKE_PREFIX_PATH})
		file(GLOB dll_files "${subdir}/bin/*.dll")
		foreach(src_file ${dll_files})
			string(REPLACE "\\" "/" src_file "${src_file}")
			install(FILES
				"${src_file}"
				DESTINATION ${CMAKE_INSTALL_BINDIR})
		endforeach(src_file ${dll_files})
	endforeach(subdir ${CMAKE_PREFIX_PATH})
else()
	install(IMPORTED_RUNTIME_ARTIFACTS mugglec)
	install(FILES
		"${OPENSSL_CRYPTO_LIBRARY}"
		DESTINATION ${CMAKE_INSTALL_LIBDIR})
	install(FILES
		"${OPENSSL_CRYPTO_LIBRARY}.3"
		DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

################################
# tests
################################
function(add_unittest name folder)
	message("add test ${name} ${folder}")
	set(name test_${name})

	file(GLOB tmp_h ${folder}/*.h)
	file(GLOB tmp_c ${folder}/*.c)

	if (MSVC OR MINGW)
		add_executable(${name} ${tmp_h} ${tmp_c})
	else()
		add_executable(${name} ${tmp_c})
	endif(MSVC OR MINGW)
	add_dependencies(${name} yoauth)
	target_link_libraries(${name} yoauth unity::framework)

	add_test(NAME ${name} COMMAND ${name})
endfunction()

if (BUILD_TESTING)
	find_package(unity)
	if (NOT unity_FOUND)
		message(FATAL_ERROR "Failed found Unity Test")
	endif()

	#enable_testing()
	include(CTest)

	set(test_root_dir ${CMAKE_CURRENT_LIST_DIR}/test)
	FILE(GLOB subdirs RELATIVE ${test_root_dir} ${test_root_dir}/*)
	FOREACH(subdir ${subdirs})
		IF(IS_DIRECTORY ${test_root_dir}/${subdir})
			add_unittest(${subdir} ${test_root_dir}/${subdir})
		ENDIF()
	ENDFOREACH()
endif()
