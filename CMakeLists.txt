cmake_minimum_required (VERSION 4.2)

project(YetAnotherRISC CXX)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)

add_subdirectory(emu)
add_subdirectory(yarisc)

include(CTest)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
