## Author(s):
##  - Corentin LE MOLGAT <clemolgat@aldebaran-robotics.com>
## Copyright (C) 2015 Aldebaran Robotics
cmake_minimum_required(VERSION 2.8)
project(flash-ov580)

# use Aldebaran builder
set(ENABLE_QIBUILD OFF)
if(qibuild_DIR)
  set(ENABLE_QIBUILD ON)
  find_package(qibuild)
else()
  set(BUILD_SHARED_LIBS ON)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dev/lib)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dev/lib)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dev/bin)
  set(CMAKE_DOCUMENTATION_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dev/share/doc)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)

# Create a executable named flash-ov580
file(GLOB_RECURSE _SRCS "src/*.[hc]pp")
if(ENABLE_QIBUILD)
  qi_create_bin(flash-ov580 ${_SRCS})
  qi_create_test(test_flash-ov580 "test/test.cpp")
else()
  add_executable(flash-ov580 ${_SRCS})
endif()
