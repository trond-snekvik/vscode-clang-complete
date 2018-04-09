cmake_minimum_required (VERSION 3.0)
project (Clang-Backend C)
set (EXECUTABLE "backend")

set(RUNTIME_OUTPUT_DIRECTORY ../lib)
option(CMAKE_BUILD_TYPE "Build type" Debug)

if (WIN32)
    set(platform "win32")
    if (NOT CLANG_DIR)
        find_program(CLANG_EXE clang)
        if (NOT CLANG_EXE STREQUAL CLANG_EXE-NOTFOUND)
            get_filename_component(CLANG_EXE_DIRECTORY CLANG_EXE DIRECTORY)
            file(TO_NATIVE_PATH "${CLANG_EXE_DIRECTORY}/.." CLANG_DIR)
        else()
            message(FATAL_ERROR "Couldn't locate libclang.")
        endif()
    endif()
    set(CLANG_RUNTIME "${CLANG_DIR}/bin/libclang.dll")
    find_library(LIBCLANG libclang
                 PATHS "${CLANG_DIR}/lib"
                 NO_DEFAULT_PATHS)
    set(node "node")
endif()

set (CMAKE_C_STANDARD 99)

find_library(LIBJANSSON jansson
             PATHS ${JANSSON_DIR}/lib
             PATH_SUFFIXES Release Debug RelWithDebInfo)
find_file(JANSSON_RUNTIME jansson.dll
             PATHS ${JANSSON_DIR}/bin
             PATH_SUFFIXES Release Debug RelWithDebInfo)

include_directories(
    "${CLANG_DIR}/include"
    "${JANSSON_DIR}/include"
    "include"
    "include/protocol"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/Collections-C/src/include"
    )

add_executable(${EXECUTABLE}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/command_handler.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/server.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/json_rpc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/indexer.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utils.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/path.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unit.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/source_file.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/doxygen.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unit_storage.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unsaved_files.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/log.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/protocol/message_handling.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/protocol/decoders.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/protocol/encoders.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/protocol/uri.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/Collections-C/src/hashtable.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/Collections-C/src/common.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/Collections-C/src/array.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/Collections-C/src/stack.c"
    )

add_definitions("-D_CRT_SECURE_NO_WARNINGS")

if (${CMAKE_BUILD_TYPE} EQUAL "Debug")
    add_definitions("-D_DEBUG")
endif()

target_link_libraries(${EXECUTABLE} PRIVATE
    ${LIBCLANG}
    ${LIBJANSSON})

set(runtime_files
    "${CLANG_RUNTIME}"
    "${JANSSON_RUNTIME}"
    )

add_custom_command(TARGET ${EXECUTABLE} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${runtime_files}
        $<TARGET_FILE_DIR:${EXECUTABLE}>)

set(GENERATED_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/include/protocol/decoders.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/protocol/encoders.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/protocol/message_handling.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/protocol/structures.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/protocol/message_handling.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/protocol/decoders.c"
)

add_custom_command(OUTPUT ${GENERATED_SOURCE_FILES}
    COMMAND ${node} "out/src/protocol_gen.js"
    DEPENDS "generator/protocol/structures.json" "${CMAKE_CURRENT_SOURCE_DIR}/../out/src/protocol_gen.js"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
    )

add_custom_target(deploy_make_dir
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/bin)

add_custom_target(deploy
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:${EXECUTABLE}>
        ${runtime_files}
        ${CMAKE_SOURCE_DIR}/bin
    DEPENDS ${EXECUTABLE} deploy_make_dir)
