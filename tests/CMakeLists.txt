find_package(Catch2 REQUIRED)

include(Catch)

add_executable(yarisc-tests
  add_test.cpp
  halt_test.cpp
  jump_test.cpp
  load_test.cpp
  machine.cpp
  machine.hpp
  move_test.cpp
  nop_test.cpp
  store_test.cpp
)

target_compile_features(yarisc-tests
  PRIVATE
    cxx_std_20
)

target_link_libraries(yarisc-tests
  PRIVATE
    Catch2::Catch2WithMain
    YetAnotherRISC:arch
    YetAnotherRISC:utils
)

target_include_directories(yarisc-tests
  PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>"
)

if(MSVC)
  set_target_properties(yarisc-tests
    PROPERTIES
      LINK_FLAGS "/ignore:4099"
  )
endif()

if(BUILD_SHARED_LIBS)
  add_custom_command(
    TARGET yarisc-tests POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_RUNTIME_DLLS:yarisc-tests> $<TARGET_FILE_DIR:yarisc-tests>
    COMMAND_EXPAND_LISTS
  )
endif()

catch_discover_tests(yarisc-tests)
