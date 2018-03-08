

include_directories(
    "${CMAKE_SOURCE_DIR}/include"
    )

add_executable(doxygen_test
    "${CMAKE_CURRENT_SOURCE_DIR}/main.c"
    "${CMAKE_SOURCE_DIR}/src/doxygen.c"
    )

add_definitions("-D_CRT_SECURE_NO_WARNINGS")
