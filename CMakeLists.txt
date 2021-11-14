cmake_minimum_required(VERSION 2.6)

set(project_name "analyze_statistics")
project(${project_name})

set(root_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
if(DEFINED OS)
	if("${OS}" STREQUAL "_UNIX_")
		if("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
                        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-elide-constructors -ggdb")
                        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-elide-constructors -ggdb")
                endif()
		add_definitions(-D_UNIX_)
		set(binary_ext "")
	elseif()
		set(binary_ext "exe")
	endif()
endif()
if(DEFINED _TEST_)
	add_definitions(-D_TEST_=${_TEST_})
endif()

file(GLOB TOP_HEADER_FILES "*.h" "*.hpp")
file(GLOB TOP_SOURCE_FILES "*.cpp")
set(binary_name "${project_name}${binary_ext}")
set(EXTERNAL_LINK_LIBRARIES sqlite3)
set(inc_dir ${root_dir}
	    ${CMAKE_CURRENT_SOURCE_DIR} 
	    ${Boost_INCLUDE_DIR}
	    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
include_directories(${inc_dir})
add_subdirectory(src)
add_subdirectory(test)
