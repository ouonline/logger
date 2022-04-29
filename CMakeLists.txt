cmake_minimum_required(VERSION 3.10)
project(logger)

# ----- options ----- #

option(LOGGER_BUILD_TESTS "build tests" ON)
option(LOGGER_INSTALL "install headers and libs" ON)

# ----- targets ----- #

file(GLOB LOGGER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
add_library(logger_static STATIC ${LOGGER_SRC})
target_include_directories(logger_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# ----- dependencies ----- #

if(NOT LOGGER_DEPS_DIR)
    set(LOGGER_DEPS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
    message(WARNING "`LOGGER_DEPS_DIR` is not set. set `LOGGER_DEPS_DIR` to ${LOGGER_DEPS_DIR}")
endif()

if(NOT TARGET cutils_static)
    set(CUTILS_BUILD_TESTS OFF CACHE BOOL "")
    set(CUTILS_DEPS_DIR ${LOGGER_DEPS_DIR})
    add_subdirectory(${LOGGER_DEPS_DIR}/cutils ${CMAKE_CURRENT_BINARY_DIR}/cutils-build)
endif()

target_link_libraries(logger_static PUBLIC cutils_static pthread)

# ----- installations ----- #

if(LOGGER_INSTALL)
    install(DIRECTORY include DESTINATION .)
    install(TARGETS logger_static DESTINATION lib)
endif()

# ----- tests ----- #

if(LOGGER_BUILD_TESTS)
    add_subdirectory(tests)
endif()
