cmake_minimum_required(VERSION 4.0)

project(stegasmlspserver)

set(CMAKE_CXX_STANDARD 23)

set(STEGASMLSPSERVER_SRC
        src/main.cpp
)

add_executable(stegasmlspserver src/main.cpp)

target_link_libraries(stegasmlspserver PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(stegasmlspserver PRIVATE stegasm_lib)