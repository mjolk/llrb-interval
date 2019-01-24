cmake_minimum_required(VERSION 3.1)
project(llrb-interval VERSION 1.0
    DESCRIPTION "Left leaning red black based interval tree"
    LANGUAGES C)

set(CMAKE_C_FLAGS_DEBUG
    "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra -Werror -Wno-long-long -Wno-variadic-macros -fexceptions -DUSE_CLANG_COMP")
enable_testing()
add_executable(llrb_test llrb-test.c)
add_test(NAME llrb_interval_test COMMAND llrb_test)
