# - CMake build/bundle script for Boost Libraries
#   Automates build of Boost, allowing optional builds of library
#   components plus CMake/pkg-config support files
#
# In current form, install Boost libraries in "tagged" layout so that
# Release/Debug/Profile Single/Multithread variants can be installed
# alongside each other. Fairly easy to modify to allow separation of
# these.

#-----------------------------------------------------------------------
# Copyright (c) 2012-2013, Ben Morgan <bmorgan.warwick@gmail.com>
# Copyright (c) 2012-2013, University of Warwick
#
# Distributed under the OSI-approved BSD 3-Clause License (the "License");
# see accompanying file License.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#-----------------------------------------------------------------------

cmake_minimum_required(VERSION 3.2)
project(BoostBuilder)

#-----------------------------------------------------------------------
# CORE CONFIGURE/BUILD OPTIONS
#-----------------------------------------------------------------------
# - CMake utilities
include(BoostBuildUtility.cmake)
include(GNUInstallDirs)

# Basic Boost Parameters
set(Boost_MAJOR_VERSION 1)
set(Boost_MINOR_VERSION 71)
set(Boost_SUBMINOR_VERSION 0)
set(Boost_VERSION "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")

# - List libraries to always build.
# We will construct the final list in the setup phase because bootstrap
# is pernickety about the --with/without-libraries being called more
# than once...
# The complete list of libraries provided by Boost can be found by
# running the bootstrap.sh script supplied with Boost as:
#   ./bootstrap.sh --with-libraries=all --show-libraries
set(Boost_INSTALLED_LIBRARIES
  date_time
  filesystem
  iostreams
  math
  program_options
  random
  serialization
  system
  test
  thread
  atomic
  chrono
  regex
  timer
  log
  )
  
set(Boost_CONFIGURE_ARGS )
set(Boost_BUILD_ARGS )



set(Boost_LIBRARY_ARGS )
foreach(_oplib ${Boost_INSTALLED_LIBRARIES}) 
	list(APPEND Boost_LIBRARY_ARGS --with-${_oplib})
endforeach()

message("boost build library is " ${Boost_LIBRARY_ARGS})

#-----------------------------------------------------------------------
# - address-model
option(boost.address64 "Build Boost 64bits library only" ON)
if(boost.address64)
  list(APPEND Boost_BUILD_ARGS  address-model=64 )
else()
  list(APPEND Boost_BUILD_ARGS  address-model=32,64 )
endif()

#-----------------------------------------------------------------------
# - Threading model
option(boost.singlethread "Build Boost single threaded library variants" OFF)
if(boost.singlethread)
  list(APPEND Boost_BUILD_ARGS  threading=multi,single )
else()
  list(APPEND Boost_BUILD_ARGS  threading=multi )
endif()

#-----------------------------------------------------------------------
# - Static libraries
option(boost.staticlibs "Build Boost static library variants" ON)
if(boost.staticlibs)
  list(APPEND Boost_BUILD_ARGS  link=static )
else()
  list(APPEND Boost_BUILD_ARGS  link=shared )
endif()

#-----------------------------------------------------------------------
# - Release/Debug/Profile libraries
# NB... To support profile libs, need to patch Boost's
# tools/build/src/tools/common.jam
# to add an entry in `local rule runtime-tag` to add a letter
# tag for profiling. Not yet clear if this rule can be overidden
# without patching

#set(__boost.buildmodes debug-symbols=on variant=release)

#option(boost.debuglibs "Build Boost debug library variants" OFF)

#if(boost.debuglibs)
#  set(__boost.buildmodes ${__boost.buildmodes},debug)
#endif()

#option(boost.profilelibs "Build boost profile library variants" OFF)
#if(boost.profilelibs)
#  set(__boost.buildmodes "${__boost.buildmodes},profile")
#endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(__boost.buildmodes debug-symbols=on variant=debug)
else()
  set(__boost.buildmodes debug-symbols=on variant=release)
endif()

list(APPEND Boost_BUILD_ARGS ${__boost.buildmodes})

#-----------------------------------------------------------------------
# - Prebuild setup
# Determine the toolset in use by CMake and create needed configure/build
# arguments and custom user-config.jam file as needed
include(${PROJECT_SOURCE_DIR}/BoostToolsetId.cmake)
Boost_Get_ToolsetId(_boost_toolset)

# Need to find out how to add flags on a per variant mode
# ... e.g. "gdwarf" etc as per
#https://cdcvs.fnal.gov/redmine/projects/build-framework/repository/boost-ssi-build/revisions/master/entry/build_boost.sh

# For GCC/Clang (could also add logic for 1y/14)
set(Boost_CXX_STANDARD_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")
# For Apple, though Mavericks onwards should defaualt to libc++
if(CMAKE_COMPILER_ID STREQUAL "AppleClang")
  set(Boost_CXX_STANDARD_FLAGS "${Boost_CXX_STANDARD_FLAGS} -stdlib=libc++")
endif()

message("ARCHS " ${ARCHS})
message("SYSROOT " ${CMAKE_OSX_SYSROOT})
set(Boost_CONFIG_EXTRA "")
if(IOS)
	set(Boost_CXX_STANDARD_FLAGS "${Boost_CXX_STANDARD_FLAGS} -arch ${ARCHS} ${CMAKE_CXX_FLAGS} -isysroot ${CMAKE_OSX_SYSROOT}")
	set(Boost_CONFIG_EXTRA "${Boost_CONFIG_EXTRA} :<striper> <root>${CMAKE_OSX_SYSROOT} <architecture>arm  <target-os>iphone ")
elseif(ANDROID)
	if (${ANDROID_ABI} STREQUAL "x86_64")
	    set(MY_SYSTEM "${CMAKE_SYSROOT}/usr/include/x86_64-linux-android")
	elseif (${ANDROID_ABI} STREQUAL "x86")
	    set(MY_SYSTEM "${CMAKE_SYSROOT}/usr/include/i686-linux-android")
	elseif (${ANDROID_ABI} STREQUAL "arm64-v8a")
	    set(MY_SYSTEM "${CMAKE_SYSROOT}/usr/include/aarch64-linux-android")
	elseif (${ANDROID_ABI} STREQUAL "armeabi-v7a")
	    set(MY_SYSTEM "${CMAKE_SYSROOT}/usr/include/arm-linux-androideabi")
	endif()
	set(Boost_CXX_STANDARD_FLAGS "${Boost_CXX_STANDARD_FLAGS} -stdlib=libc++ ${CMAKE_CXX_FLAGS} -isysroot ${CMAKE_SYSROOT} -isystem ${MY_SYSTEM} ${ANDROID_LINKER_FLAGS}  --target=${CMAKE_CXX_COMPILER_TARGET} --gcc-toolchain=${CMAKE_CXX_COMPILER_EXTERNAL_TOOLCHAIN}")
	set(Boost_CONFIG_EXTRA "${Boost_CONFIG_EXTRA} :<target-os>android ")
elseif(APPLE)
  set(Boost_CXX_STANDARD_FLAGS "${Boost_CXX_STANDARD_FLAGS} -stdlib=libc++ -isysroot ${CMAKE_OSX_SYSROOT}")
endif()

message("Boost_CXX_STANDARD_FLAGS " ${Boost_CXX_STANDARD_FLAGS})
if(_boost_toolset)
  list(APPEND Boost_CONFIGURE_ARGS "toolset=${_boost_toolset}")
  list(APPEND Boost_BUILD_ARGS     "toolset=${_boost_toolset}")

  set(Boost_USER_CONFIG_JAMFILE "${PROJECT_BINARY_DIR}/user-config.jam")

if(ANDROID)
  set(ANDROID_CXX "/Users/funplus/Documents/android-ndk-r19/toolchains/llvm/prebuilt/darwin-x86_64/bin/aarch64-linux-android21-clang++")
  set(ARPATH "/Users/funplus/Documents/android-ndk-r19/toolchains/llvm/prebuilt/darwin-x86_64/bin/aarch64-linux-android-ar")
  set(RANLIB "/Users/funplus/Documents/android-ndk-r19/toolchains/llvm/prebuilt/darwin-x86_64/bin/aarch64-linux-android-ranlib")
  file(WRITE "${Boost_USER_CONFIG_JAMFILE}" "using ${_boost_toolset} : : ${ANDROID_CXX} : <archiver>${ARPATH} <ranlib>${RANLIB} <cxxflags>\"${Boost_CXX_STANDARD_FLAGS}\" <linkflags>\"${Boost_CXX_STANDARD_FLAGS}\" ${Boost_CONFIG_EXTRA};")
else()
  file(WRITE "${Boost_USER_CONFIG_JAMFILE}" "using ${_boost_toolset} : : ${CMAKE_CXX_COMPILER} : <cxxflags>\"${Boost_CXX_STANDARD_FLAGS}\" <linkflags>\"${Boost_CXX_STANDARD_FLAGS}\" ${Boost_CONFIG_EXTRA};")
endif()

  set(Boost_BUILD_USER_CONFIG_ARGS "--user-config=${PROJECT_BINARY_DIR}/user-config.jam")
  list(APPEND Boost_BUILD_ARGS     "${Boost_BUILD_USER_CONFIG_ARGS}")
endif()

# Construct the final library list to install
#join_list(Boost_LIBRARY_ARGS " " Boost_WITHLIBRARY_LIST)
list(APPEND Boost_CONFIGURE_ARGS ${Boost_LIBRARY_ARGS})
list(APPEND Boost_BUILD_ARGS ${Boost_LIBRARY_ARGS})

# Parallelize build if possible
include(ProcessorCount)
ProcessorCount(NJOBS)
if(NJOBS EQUAL 0)
  set(NJOBS 1)
endif()

#-----------------------------------------------------------------------
# Build Boost
#
include(ExternalProject)
if(WIN32)
set(BOOST_SROUCE ${PROJECT_SOURCE_DIR}/boost_1_71_0.zip)
set(BOOST_STRAP <SOURCE_DIR>/bootstrap.bat)
set(BOOST_BUILD "")
else()
set(BOOST_SROUCE ${PROJECT_SOURCE_DIR}/boost_1_71_0.tar.gz)
set(BOOST_BUILD "")
set(BOOST_STRAP <SOURCE_DIR>/bootstrap.sh)
endif()
message("BOOST CONFIGURE ARGS is " ${Boost_CONFIGURE_ARGS})
message("BOOST BUILD ARGS is " ${Boost_BUILD_ARGS})
message(${CMAKE_INSTALL_PREFIX})
message(${CMAKE_INSTALL_FULL_LIBDIR})
ExternalProject_Add(boost
  #URL                 "https://downloads.sourceforge.net/project/boost/boost/1.59.0/boost_1_59_0.tar.bz2"
  #URL_HASH            SHA256=727a932322d94287b62abb1bd2d41723eec4356a7728909e38adb65ca25241ca

  # If sourceforge downloads barf, copy by hand and stick it next to this
  URL                 ${BOOST_SROUCE}

  BUILD_IN_SOURCE     1
  CONFIGURE_COMMAND   ${BOOST_STRAP}
                      
  # All in one build/install step is slightly faster
  BUILD_COMMAND       ""
  INSTALL_COMMAND     ./b2 -d2 -q -j${NJOBS} install
					  --prefix=${CMAKE_INSTALL_PREFIX}
                      ${Boost_BUILD_ARGS}
  STEP_TARGETS        download update configure install
  )



