cmake_minimum_required(VERSION 4.0)

project(steglsp)

set(APP_NAME steglsp)

set(CMAKE_CXX_STANDARD 23)

set(STEGLSP_SRC
        ./main.cpp
        core/Transport.cpp
        core/Transport.h
        core/Server.cpp
        core/Server.h
        handlers/Initialize.cpp
        handlers/Initialize.h
        handlers/Shutdown.cpp
        handlers/Shutdown.h
        handlers/Exit.cpp
        handlers/Exit.h
        handlers/DidOpen.cpp
        handlers/DidOpen.h
        handlers/SemanticTokensFull.cpp
        handlers/SemanticTokensFull.h
        lsp/Tokens.cpp
        lsp/Tokens.h
        handlers/DidChange.cpp
        handlers/DidChange.h
        lsp/DocumentStore.h
        lsp/Diagnostics.cpp
        lsp/Diagnostics.h
)

add_executable(steglsp ${STEGLSP_SRC})

target_include_directories(steglsp PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(steglsp PRIVATE
        steg_lib
        nlohmann_json::nlohmann_json
)