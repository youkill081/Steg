cmake_minimum_required(VERSION 4.0)

project(stegnocode)

set(CMAKE_CXX_STANDARD 23)

set(LIB_SOURCES
        images/pixel.h
        images/Pixels.h
        images/Image.cpp
        images/Image.h
        steganographer/Steganographer.cpp
        steganographer/Steganographer.h
        steganographer/Random32.h
        steganographer/SequenceManager.cpp
        steganographer/SequenceManager.h
        steganographer/sequences/WriteSequence.cpp
        steganographer/sequences/WriteSequence.h
        steganographer/constant.h
        steganographer/sequences/ReadSequence.cpp
        steganographer/sequences/ReadSequence.h
        steganographer/Header.hpp
        utils/Parameters.cpp
        utils/Parameters.h
        handlers/IHander.h
        handlers/StringHandler.cpp
        handlers/StringHandler.h
        handlers/handlers.h
        handlers/FileHandler.cpp
        handlers/FileHandler.h
        handlers/CodeHandler.cpp
        handlers/CodeHandler.h
)

add_library(stegnocode_lib STATIC ${LIB_SOURCES})

target_link_libraries(stegnocode_lib PUBLIC stegcommon) # Link stegcommon
target_link_libraries(stegnocode_lib PUBLIC stegasm_lib) # Link stegasm

target_include_directories(stegnocode_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/steganographer
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/stb
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/stegcommon
)

