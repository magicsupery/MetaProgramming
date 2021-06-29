cmake_minimum_required(VERSION 3.2)
set(projectName MetaProgramming)

project(${projectName})

# build config
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

# 
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(BUILD_SHARED_LIBS "Build shared libs" OFF)


if (CMAKE_BUILD_TYPE)
else()
	set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()


if (UNIX)
    set(CMAKE_CXX_FLAGS "-fPIC -Wall -m64" ${CMAKE_CXX_FLAGS})
elseif (WIN32)
    
endif()


#build with big file 
if(MINGW)
	set(CMAKE_CXX_FLAGS "-Wa,-mbig-obj -O2" ${CMAKE_CXX_FLAGS})
endif()

if(MSVC)
	add_compile_options( /bigobj )
endif()

if(WIN32)
	add_definitions(-D_WIN32_WINNT=0x600)
endif()
message("------------ Options -------------")
message("  CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message("  CMAKE_COMMAND: ${CMAKE_COMMAND}")
message("  CMAME_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message("-------------- Env ---------------")
message("  CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message("  CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message("  CMAKE_PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
message("----------------------------------")


include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/install/include)

#lib
link_directories(${PROJECT_SOURCE_DIR}/3rdparty/install/lib)
link_directories(${PROJECT_SOURCE_DIR}/3rdparty/install/lib64)
link_directories(${PROJECT_BINARY_DIR}/lib)


add_subdirectory(meta)
