cmake_minimum_required(VERSION 3.16)

project(
    ioport_emu
    VERSION 0.1.0
    DESCRIPTION "Archived x86 I/O port emulation core"
    LANGUAGES CXX
)

option(IOPORT_EMU_BUILD_TESTS "Build ioport_emu characterization tests" ON)

add_library(ioport_emu STATIC
    src/Decoder.cpp
    src/HandlerRegistry.cpp
    src/IOPortEmulator.cpp
)

target_compile_features(ioport_emu PUBLIC cxx_std_17)
target_include_directories(ioport_emu
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if(MSVC)
    target_compile_options(ioport_emu PRIVATE /W4 /permissive-)
else()
    target_compile_options(ioport_emu PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(IOPORT_EMU_BUILD_TESTS)
    enable_testing()

    add_executable(ioport_emu_tests
        tests/ioport_emu_tests.cpp
    )

    target_link_libraries(ioport_emu_tests PRIVATE ioport_emu)

    add_test(NAME ioport_emu_tests COMMAND ioport_emu_tests)
endif()
