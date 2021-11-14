cmake_minimum_required(VERSION 2.6)

project(test)

#source_group(HEADER_FILES FILES ${HEADER_FILES})
#source_group(SOURCE_FILES FILES ${SOURCE_FILES})
set(HEADER_FILES FILES ${HEADER_FILES})
set(SOURCE_FILES FILES ${SOURCE_FILES})

find_package(Boost 1.74.0 COMPONENTS program_options REQUIRED)
set(inc_dir ${root_dir})
set(test_sources ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_MULTITHREADED ON)
find_package (Boost COMPONENTS unit_test_framework REQUIRED)
include_directories(${inc_dir} ${Boost_INCLUDE_DIRS})
set(test ${binary_name}_unit_tests)
add_executable (${test} ${test_sources})
target_link_libraries (${test} ${Boost_LIBRARIES} ${EXTERNAL_LINK_LIBRARIES})
add_test (${test} ${test})
enable_testing()

