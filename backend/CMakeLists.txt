cmake_minimum_required (VERSION 3.0)
project (Clang-Backend C CXX)
set (EXECUTABLE "backend")

if (WIN32)
    set(platform "win32")
    find_library(LIBCLANG libclang lib/clang/${platform}/bin)
    set(node "node")
elseif(UNIX)
    set(platform "unix")
    set(node "nodejs")
    link_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib/clang/${platform}/bin")
endif()

set (CMAKE_C_STANDARD 99)

find_library(LIBJANSSON jansson lib/jansson/${platform}/bin)

include_directories(
    "lib/clang/${platform}/include"
    "lib/jansson/${platform}/include"
    "include"
    "include/protocol"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/Collections-C/src/include"
    ${CURSES_INCLUDE_DIR}
    )

add_executable(${EXECUTABLE}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/command_handler.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/server.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/json_rpc.c"
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

set_property(TARGET ${EXECUTABLE} PROPERTY CXX_STANDARD 11)

if (WIN32)
    add_definitions(
        "-D_CRT_SECURE_NO_WARNINGS"
        "-D_DEBUG")
    target_link_libraries(${EXECUTABLE} PRIVATE
        ${LIBCLANG}
        ${LIBJANSSON})

elseif (UNIX)

    target_link_libraries(${EXECUTABLE}
        clang
        clangFrontend
        clangSerialization
        clangDriver
        clangParse
        clangSema
        clangAnalysis
        clangAST
        clangBasic
        clangEdit
        clangLex
        clangTooling
    )

    # target_link_libraries(${EXECUTABLE}
    #     LLVMX86AsmParser # MC, MCParser, Support, X86Desc, X86Info
    #     LLVMX86Desc # MC, Support, X86AsmPrinter, X86Info
    #     LLVMX86AsmPrinter # MC, Support, X86Utils
    #     LLVMX86Info # MC, Support, Target
    #     LLVMX86Utils # Core, Support
    #     LLVMipo
    #     LLVMScalarOpts
    #     LLVMInstCombine
    #     LLVMTransformUtils
    #     LLVMAnalysis
    #     LLVMTarget
    #     LLVMOption # Support
    #     LLVMMCParser # MC, Support
    #     LLVMMC # Object, Support
    #     LLVMObject # BitReader, Core, Support
    #     LLVMBitReader # Core, Support
    #     LLVMCore # Support
    #     LLVMSupport
    # )

    include(FindCurses)
    target_link_libraries(${EXECUTABLE}
        ${LIBJANSSON}
        pthread
        z
        dl
        tinfo
        ${CURSES_LIBRARIES}
    )
endif()

if (WIN32)
    add_custom_command(TARGET ${EXECUTABLE} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/lib/clang/bin/libclang.dll"
            $<TARGET_FILE_DIR:${EXECUTABLE}>)

    add_custom_command(TARGET ${EXECUTABLE} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/lib/jansson/bin/jansson.dll"
            $<TARGET_FILE_DIR:${EXECUTABLE}>)
endif()

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