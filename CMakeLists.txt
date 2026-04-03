cmake_minimum_required(VERSION 4.0)

project(stegnocode)

set(APP_NAME stegnocode)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(JSON_BuildTests OFF CACHE INTERNAL "")

add_subdirectory(libs/stegcommon)
add_subdirectory(libs/json)
add_subdirectory(libs/raylib)

add_subdirectory(steg)
add_subdirectory(stegasm)
add_subdirectory(steg/lsp)
add_subdirectory(steganographer)

add_subdirectory(test)