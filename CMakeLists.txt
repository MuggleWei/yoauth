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
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-missing-field-initializers")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-missing-field-initializers")
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
option(BUILD_SANITIZER "Build with sanitizer" OFF)
set(CMAKE_INSTALL_LIBDIR lib)

# build flags
message("# BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
message("# BUILD_TESTING: ${BUILD_TESTING}")
message("# BUILD_SANITIZER: ${BUILD_SANITIZER}")

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

find_package(OpenSSL 3 REQUIRED)
if (OPENSSL_FOUND)
	message("deps openssl version: ${OPENSSL_VERSION}")
	message("deps openssl include dir: ${OPENSSL_INCLUDE_DIR}")
	message("deps openssl crypto lib: ${OPENSSL_CRYPTO_LIBRARY}")
	message("deps openssl ssl lib: ${OPENSSL_SSL_LIBRARY}")
else()
	message(FATAL_ERROR "Can't find openssl")
endif()

#include(${CMAKE_CURRENT_LIST_DIR}/cmake/FindNcurses.cmake)
#if (NCURSES_FOUND)
#    message("find ncurses include dir: ${NCURSES_INCLUDE_DIR}")
#    message("find ncurses lib: ${NCURSES_LIBRARIES}")
#else()
#    message(FATAL_ERROR "Can't find ncurses")
#endif()

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
set(yoauth_core yoauth_core)
FILE(GLOB_RECURSE yoauth_core_src "${yoauth_src_dir}/yoauth/core/*.c")
FILE(GLOB_RECURSE yoauth_core_h "${yoauth_src_dir}/yoauth/core/*.h")
add_library(${yoauth_core} ${YOAUTH_LIB_TYPE}
	${yoauth_core_src}
	${yoauth_core_h}
	${yoauth_gen_h})
target_include_directories(${yoauth_core} PUBLIC
	$<BUILD_INTERFACE:${yoauth_src_dir}>
	$<BUILD_INTERFACE:${yoauth_gen_dir}>
	$<INSTALL_INTERFACE:include>)
target_link_libraries(${yoauth_core} mugglec "${OPENSSL_CRYPTO_LIBRARY}")
if (UNIX)
	target_link_libraries(${yoauth_core} m)
endif()
target_compile_definitions(${yoauth_core}
	PRIVATE YOAUTH_EXPORTS MUGGLE_HOLD_LOG_MACRO)
set_target_properties(${yoauth_core} PROPERTIES
	DEBUG_POSTFIX d
	VERSION ${YOAUTH_VERSION}
	SOVERSION ${YOAUTH_VERSION_MAJOR})

if (BUILD_SANITIZER)
	if ((${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU") OR
		(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"))
		target_compile_options(${yoauth_core} PUBLIC
			-fsanitize=undefined
			-fsanitize=address
			-fsanitize=leak)
		target_link_options(${yoauth_core} PUBLIC
			-fsanitize=undefined
			-fsanitize=address
			-fsanitize=leak)
	endif()
endif()

if (APPLE)
	set_target_properties(${yoauth_core}
		PROPERTIES
		INSTALL_RPATH "@executable_path/../lib")
elseif (UNIX)
	set_target_properties(${yoauth_core}
		PROPERTIES
		INSTALL_RPATH "\$ORIGIN/../lib")
endif()

# exe
set(yoauth_exe yoauth)
FILE(GLOB_RECURSE yoauth_exe_c "${yoauth_src_dir}/yoauth/exe/*.c")
FILE(GLOB_RECURSE yoauth_exe_h "${yoauth_src_dir}/yoauth/exe/*.h")
add_executable(${yoauth_exe} ${yoauth_exe_c} ${yoauth_exe_h})
add_dependencies(${yoauth_exe} ${yoauth_core})
target_include_directories(${yoauth_exe} PUBLIC
	${NCURSES_INCLUDE_DIR})
target_link_libraries(${yoauth_exe} ${yoauth_core} ${NCURSES_LIBRARIES})
target_compile_definitions(${yoauth_exe}
	PUBLIC MUGGLE_HOLD_LOG_MACRO)
set_target_properties(${yoauth_exe} PROPERTIES
	DEBUG_POSTFIX d
	VERSION ${YOAUTH_VERSION}
	SOVERSION ${YOAUTH_VERSION_MAJOR})

if (BUILD_SANITIZER)
	if ((${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU") OR
		(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"))
		target_compile_options(${yoauth_exe} PUBLIC
			-fsanitize=undefined
			-fsanitize=address
			-fsanitize=leak)
		target_link_options(${yoauth_exe} PUBLIC
			-fsanitize=undefined
			-fsanitize=address
			-fsanitize=leak)
	endif()
endif()

if (MSVC OR MINGW)
	set_target_properties(${yoauth_exe}
		PROPERTIES
		VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)")
else()
	if (APPLE)
		set_target_properties(${yoauth_exe}
			PROPERTIES
			INSTALL_RPATH "@executable_path/../lib")
	elseif (UNIX)
		set_target_properties(${yoauth_exe}
			PROPERTIES
			INSTALL_RPATH "\$ORIGIN/../lib")
	endif()
endif()

# for debug
function(muggle_copy_dir_target name src_dir dst_dir)
	file(GLOB_RECURSE src_files "${src_dir}/*")
	foreach(src_file ${src_files})
		file(RELATIVE_PATH rel_path ${src_dir} ${src_file})
		set(dst_file "${dst_dir}/${rel_path}")
		add_custom_command(
			OUTPUT "${dst_file}"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${src_file}" ${dst_file}
			DEPENDS "${src_file}"
			COMMENT "copy ${src_file} to ${dst_file}"
		)
		list(APPEND dst_files "${dst_file}")
	endforeach()
	add_custom_target(${name} ALL DEPENDS ${dst_files})
endfunction()

if (WIN32)
	foreach(subdir ${CMAKE_PREFIX_PATH})
		file(GLOB dll_files "${subdir}/bin/*.dll")
		foreach(src_file ${dll_files})
			file(RELATIVE_PATH rel_path ${src_file} ${subdir}/bin)
			get_filename_component(dst_name ${src_file} NAME)
			set(dst_file "$(OutDir)${dst_name}")
			add_custom_command(
				TARGET ${yoauth_core}
				POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different "${src_file}" ${dst_file}
				DEPENDS "${src_file}"
				COMMENT "sync ${src_file} to ${dst_file}"
			)
		endforeach(src_file ${dll_files})
	endforeach(subdir ${CMAKE_PREFIX_PATH})
else()
	# # ncurse share resources
	# foreach(search_dir ${CMAKE_PREFIX_PATH})
	# 	file(GLOB children RELATIVE "${search_dir}" "${search_dir}/*")
	# 	foreach(child ${children})
	# 		if ("${child}" STREQUAL "share")
	# 			file(GLOB share_dirs RELATIVE "${search_dir}/${child}" "${search_dir}/${child}/*")
	# 			foreach(share_dir ${share_dirs})
	# 				if (("${share_dir}" STREQUAL "tabset") OR ("${share_dir}" STREQUAL "terminfo"))
	# 					set(res_path "${search_dir}/${child}/${share_dir}")
	# 					set(dst_path "${CMAKE_BINARY_DIR}/share/${share_dir}")
	# 					add_custom_command(
	# 						TARGET ${yoauth_exe}
	# 						POST_BUILD
	# 						COMMAND ${CMAKE_COMMAND} -E copy_directory "${res_path}" "${dst_path}"
	# 						DEPENDS "${res_path}"
	# 						COMMENT "sync ${res_path} to ${dst_path}"
	# 						)
	# 				endif()
	# 			endforeach()
	# 		endif()
	# 	endforeach()
	# endforeach(search_dir ${CMAKE_PREFIX_PATH})
endif()

################################
# install
################################
include(GNUInstallDirs)
install(TARGETS ${yoauth_core} ${yoauth_exe}
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
	foreach(subdir ${CMAKE_PREFIX_PATH})
		file(GLOB shared_files "${subdir}/lib/*.so*")
		foreach(src_file ${shared_files})
			string(REPLACE "\\" "/" src_file "${src_file}")
			install(FILES
				"${src_file}"
				DESTINATION ${CMAKE_INSTALL_LIBDIR})
		endforeach(src_file ${shared_files})
	endforeach(subdir ${CMAKE_PREFIX_PATH})
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
	add_dependencies(${name} ${yoauth_core})
	target_link_libraries(${name} ${yoauth_core} unity::framework)

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
