cmake_minimum_required(VERSION 3.2)
project(geno_pattern)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src GENO_PATTERN_SOURCE_FILES)

add_executable(geno_pattern ${GENO_PATTERN_SOURCE_FILES})
