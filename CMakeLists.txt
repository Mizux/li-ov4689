## Author(s):
##  - Corentin LE MOLGAT <clemolgat@aldebaran-robotics.com>
## Copyright (C) 2015 Aldebaran Robotics
cmake_minimum_required(VERSION 3.2)
project(TestFirmware)

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

add_subdirectory(libcamera)

# Problems on both cameras
add_subdirectory(camera_startup)
add_subdirectory(camera_queue)

# Problem specific to ov4689
add_subdirectory(ov4689_full)
add_subdirectory(ov4689_longterm)
add_subdirectory(ov4689_timestamp)
add_subdirectory(ov4689_images)
add_subdirectory(ov4689_images_latency)

# Problem specific to ov5640
add_subdirectory(ov5640_exposure)
add_subdirectory(ov5640_flip)
add_subdirectory(ov5640_ext)
