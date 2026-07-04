cmake_minimum_required(VERSION 4.3)
project(llrb-interval VERSION 1.0.0
    DESCRIPTION "Left leaning red black based interval tree"
    LANGUAGES C)

set(CMAKE_C_FLAGS_DEBUG
    "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra -Werror -Wno-long-long -Wno-variadic-macros -fexceptions -DUSE_CLANG_COMP")

add_library(llrb-interval INTERFACE)
target_include_directories(llrb-interval INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(llrb-interval PROPERTIES 
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  PUBLIC_HEADER "llrb-interval.h;llrb.h;queue.h")

install(TARGETS llrb-interval 
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
configure_file(llrb-interval.pc.in llrb-interval.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/llrb-interval.pc DESTINATION
  ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
enable_testing()
add_executable(llrb_test llrb-test.c)
target_link_libraries(llrb_test llrb-interval)
add_test(NAME llrb_interval_test COMMAND llrb_test)
