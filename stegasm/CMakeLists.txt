cmake_minimum_required(VERSION 4.0)

project(stegasm)

set(CMAKE_CXX_STANDARD 23)

set(SOURCES
        src/main.cpp
        src/instructions.h
        src/interpreter/runtime/Runtime.h
        src/interpreter/instructions/instruction.h
        src/compiler/TextParser.cpp
        src/compiler/TextParser.h
        src/compiler/Compiler.cpp
        src/compiler/Compiler.h
        src/instruction_utils.hpp
        src/interpreter/Loader.cpp
        src/interpreter/Loader.h
        src/interpreter/runtime/Memory.h
        src/interpreter/Vm.cpp
        src/interpreter/Vm.h
        src/interpreter/runtime/Memory.cpp
        src/interpreter/exceptions.h
)

add_executable(stegasm ${SOURCES})

target_link_libraries(stegasm PRIVATE stegcommon)
target_include_directories(stegasm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../libs/stegcommon)
