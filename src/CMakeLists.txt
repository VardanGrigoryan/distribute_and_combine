cmake_minimum_required(VERSION 2.6)

project(engine)

file(GLOB HEADER_FILES "*.h" "*.hpp")
file(GLOB SOURCE_FILES "*.cpp")
set(HEADER_FILES ${TOP_HEADER_FILES} ${HEADER_FILES})
set(SOURCE_FILES ${TOP_SOURCE_FILES} ${SOURCE_FILES})

find_package (Boost COMPONENTS program_options REQUIRED)
include_directories(${inc_dir} ${Boost_INCLUDE_DIR})
add_executable(${binary_name} ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries (${binary_name} LINK_PUBLIC ${EXTERNAL_LINK_LIBRARIES} ${Boost_LIBRARIES})
set_source_files_properties(${SOURCE_FILES} PROPERTIES LANGUAGE CXX)

