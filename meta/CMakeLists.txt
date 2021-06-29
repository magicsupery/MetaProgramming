project(meta)

option(BUILD_TEST "build test or not" ON)
set(SRC_LIST )

macro(add_src_dir name)
	file(GLOB files ${name}/*.h ${name}/*.cpp ${name}/*.hpp ${name}/*.cc ${name}/*.c)
		if(WIN32)
			list(FILTER files EXCLUDE REGEX _linux.cpp)
		else()
			list(FILTER files EXCLUDE REGEX _win.cpp)
		endif()
	
	foreach(file IN LISTS files)
		set(SRC_LIST ${file} ${SRC_LIST})
	endforeach(file)
	
endmacro()




add_src_dir(.)


set(Boost_USE_STATIC_LIBS ON)
set(Boost_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/install/lib/cmake/Boost-1.71.0)
if(MINGW)
	set(Boost_COMPILER mgw81)
endif()
find_package(Boost 1.71 REQUIRED COMPONENTS unit_test_framework)

include_directories(${LUA_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIRS})


if (MSVC)
	set(_MODULE_LINK ${Boost_LIBRARIES}${_MODULE_LINK}  spdlog)
else()
	set(_MODULE_LINK ${Boost_LIBRARIES}${_MODULE_LINK}  spdlog)
endif()

if(WIN32)
    # Win32 modules need to be linked to the Lua library.
    set(_MODULE_LINK ${LUA_LIBRARIES} ${_MODULE_LINK} ws2_32 wsock32)
else()

endif()

#set(TARGET meta)
#add_library(${TARGET} MODULE ${SRC_LIST})
#set_target_properties(${TARGET} PROPERTIES PREFIX "")
#target_link_libraries(${TARGET} ${_MODULE_LINK})
#
#if(WIN32)
#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_LIST})
#endif()
#
#
#install(TARGETS ${TARGET} LIBRARY DESTINATION ${LIBRARY_OUTPUT_PATH} ARCHIVE DESTINATION "lib")
if(BUILD_TEST)
	add_subdirectory(UnitTest)
endif()
