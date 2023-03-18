add_library(yarisc-utils
  color.cpp
  color.hpp
  ios.hpp
)

add_library(YetAnotherRISC:utils ALIAS yarisc-utils)

target_compile_features(yarisc-utils
  PUBLIC
    cxx_std_20
)

target_include_directories(yarisc-utils
  PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>"
    "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/codegen>"
)

set_target_properties(yarisc-utils
  PROPERTIES
    DEFINE_SYMBOL YARISC_UTILS_EXPORTS
    C_VISIBILITY_PRESET hidden
    CXX_EXTENSIONS OFF
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    WINDOWS_EXPORT_ALL_SYMBOLS OFF
)

generate_export_header(yarisc-utils
  BASE_NAME YARISC_UTILS
  EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/codegen/yarisc/utils/export.h"
  INCLUDE_GUARD_NAME YARISC_UTILS_EXPORT_H
)
