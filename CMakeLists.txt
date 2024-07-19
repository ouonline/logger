cmake_minimum_required(VERSION 3.14)
project(logger)

# ----- options ----- #

option(LOGGER_BUILD_TESTS "build tests" ON)
option(LOGGER_INSTALL "install headers and libs" ON)

# ----- targets ----- #

file(GLOB_RECURSE __LOGGER_SRC__ ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
add_library(logger_static STATIC ${__LOGGER_SRC__})
unset(__LOGGER_SRC__)

target_include_directories(logger_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(MSVC)
    target_compile_options(logger_static PRIVATE /W4)
else()
    target_compile_options(logger_static PRIVATE -Wall -Wextra -Werror -Wno-format-overflow)
endif()

# ----- dependencies ----- #

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

if(NOT LOGGER_DEPS_DIR)
    get_filename_component(LOGGER_DEPS_DIR ${CMAKE_CURRENT_BINARY_DIR}/deps ABSOLUTE)
    message("-- `LOGGER_DEPS_DIR` is not set. set `LOGGER_DEPS_DIR` to `${LOGGER_DEPS_DIR}`.")
endif()

if(NOT TARGET cutils_static)
    set(CUTILS_BUILD_TESTS OFF CACHE BOOL "")
    set(CUTILS_DEPS_DIR ${LOGGER_DEPS_DIR})
    FetchContent_Declare(cutils
        GIT_REPOSITORY "https://github.com/ouonline/cutils.git"
        GIT_TAG master
        SOURCE_DIR ${LOGGER_DEPS_DIR}/cutils
        BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/cutils-build
        SUBBUILD_DIR ${LOGGER_DEPS_DIR}/cutils-subbuild)
    FetchContent_MakeAvailable(cutils)
endif()

target_link_libraries(logger_static PUBLIC cutils_static)

# ----- installations ----- #

if(LOGGER_INSTALL)
    install(DIRECTORY include DESTINATION .)
    install(TARGETS logger_static DESTINATION lib)
endif()

# ----- tests ----- #

if(LOGGER_BUILD_TESTS)
    add_subdirectory(tests)
endif()
