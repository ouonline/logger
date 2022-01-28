cmake_minimum_required(VERSION 3.10)
project(logger)

option(LOGGER_BUILD_TESTS "build tests" ON)

file(GLOB LOGGER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
add_library(logger_static ${LOGGER_SRC})
target_include_directories(logger_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(logger_static PUBLIC pthread)

set(CUTILS_BUILD_TESTS OFF CACHE BOOL "")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../cutils ${CMAKE_CURRENT_BINARY_DIR}/cutils)
target_link_libraries(logger_static PUBLIC cutils_static)

if(LOGGER_BUILD_TESTS)
    add_subdirectory(tests)
endif()
