add_library(yarisc-arch
  assembly.cpp
  assembly.hpp
  debugger.cpp
  debugger.hpp
  feature_level.hpp
  instructions.hpp
  machine.cpp
  machine.hpp
  machine_model.cpp
  machine_model.hpp
  machine_profile.hpp
  memory.cpp
  memory.hpp
  output.hpp
  registers.hpp
  types.hpp
  detail/colors.hpp
  detail/endianness.hpp
  detail/execution.hpp
  detail/format.hpp
  detail/functional.hpp
  detail/hex_memory.hpp
  detail/hex_registers.hpp
  detail/hex_word.hpp
  detail/status_bits.hpp
)

add_library(YetAnotherRISC:arch ALIAS yarisc-arch)

target_compile_features(yarisc-arch
  PUBLIC
    cxx_std_20
)

target_link_libraries(yarisc-arch
  PRIVATE
    YetAnotherRISC:utils
)

target_include_directories(yarisc-arch
  PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>"
    "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/codegen>"
)

set_target_properties(yarisc-arch
  PROPERTIES
    DEFINE_SYMBOL YARISC_ARCH_EXPORTS
    C_VISIBILITY_PRESET hidden
    CXX_EXTENSIONS OFF
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    WINDOWS_EXPORT_ALL_SYMBOLS OFF
)

generate_export_header(yarisc-arch
  BASE_NAME YARISC_ARCH
  EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/codegen/yarisc/arch/export.h"
  INCLUDE_GUARD_NAME YARISC_ARCH_EXPORT_H
)
