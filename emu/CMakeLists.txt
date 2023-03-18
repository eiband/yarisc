add_executable(yarisc-emu
  main.cpp
  emulator.cpp
  emulator.hpp
)

target_compile_features(yarisc-emu
  PRIVATE
    cxx_std_20
)

target_link_libraries(yarisc-emu
  PRIVATE
    YetAnotherRISC:arch
    YetAnotherRISC:utils
)

target_include_directories(yarisc-emu
  PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>"
)

if(BUILD_SHARED_LIBS)
  add_custom_command(
    TARGET yarisc-emu POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_RUNTIME_DLLS:yarisc-emu> $<TARGET_FILE_DIR:yarisc-emu>
    COMMAND_EXPAND_LISTS
  )
endif()
