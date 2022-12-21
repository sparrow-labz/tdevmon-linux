project(devmon_b)
cmake_minimum_required(VERSION 2.8)

include(axl/cmake/axl_cmake.cmake NO_POLICY_SCOPE)

add_subdirectory(axl)
include(axl/cmake/import_axl.cmake)

add_subdirectory(devmon)
