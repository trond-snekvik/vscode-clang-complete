

include_directories(
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/lib/Collections-C/src/include"
    )

add_executable(path_test
    "${CMAKE_CURRENT_SOURCE_DIR}/main.c"
    "${CMAKE_SOURCE_DIR}/src/path.c"
    "${CMAKE_SOURCE_DIR}/lib/Collections-C/src/common.c"
    "${CMAKE_SOURCE_DIR}/lib/Collections-C/src/array.c"
    "${CMAKE_SOURCE_DIR}/lib/Collections-C/src/stack.c"
    )

add_definitions("-D_CRT_SECURE_NO_WARNINGS")
