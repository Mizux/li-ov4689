set(_NAME camera)

file(GLOB_RECURSE _HDRS "include/*.hpp")
file(GLOB_RECURSE _SRCS "src/*.[hc]pp")

if(ENABLE_QIBUILD)
	include_directories("include")
  qi_create_lib(${_NAME} ${_HDRS} ${_SRCS})
  qi_stage_lib(${_NAME})
else()
  add_library(${_NAME} ${_HDRS} ${_SRCS})
	target_include_directories(${_NAME} PUBLIC include)
endif()
