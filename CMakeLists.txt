cmake_minimum_required(VERSION 3.14)
project(logger)

# ----- options ----- #

option(LOGGER_BUILD_TESTS "build tests" ON)
option(LOGGER_INSTALL "install headers and libs" ON)
option(LOGGER_HOLD_DEPS "do not update existing deps" OFF)
option(LOGGER_STDOUT_ANSI_COLOR "use ansi color in stdout" ON)

# ----- dependencies ----- #

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED ${LOGGER_HOLD_DEPS})

if(NOT LOGGER_DEPS_DIR)
    get_filename_component(LOGGER_DEPS_DIR ${CMAKE_CURRENT_BINARY_DIR}/deps ABSOLUTE)
    message(STATUS "`LOGGER_DEPS_DIR` is not set. set `LOGGER_DEPS_DIR` to `${LOGGER_DEPS_DIR}` by default.")
endif()

macro(logger_import_git_dep name repo commit)
    FetchContent_Declare(${name}
        GIT_REPOSITORY ${repo}
        GIT_TAG ${commit}
        GIT_SHALLOW TRUE
        SOURCE_DIR ${LOGGER_DEPS_DIR}/${name}
        BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${name}-build
        SUBBUILD_DIR ${LOGGER_DEPS_DIR}/${name}-subbuild)
    FetchContent_MakeAvailable(${name})
endmacro()

if(NOT TARGET cutils_static)
    set(CUTILS_DEPS_DIR ${LOGGER_DEPS_DIR})
    set(CUTILS_BUILD_TESTS OFF)
    set(CUTILS_INSTALL ${LOGGER_INSTALL})
    set(CUTILS_HOLD_DEPS ${LOGGER_HOLD_DEPS})
    if(LOGGER_DEP_CUTILS_SOURCE_DIR)
        add_subdirectory(${LOGGER_DEP_CUTILS_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/cutils-build)
        set(cutils_SOURCE_DIR ${LOGGER_DEP_CUTILS_SOURCE_DIR})
    else()
        logger_import_git_dep(cutils "https://github.com/ouonline/cutils.git" master)
    endif()
endif()

# ----- targets ----- #

file(GLOB __LOGGER_SRC__ ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
add_library(logger_static STATIC ${__LOGGER_SRC__})
unset(__LOGGER_SRC__)

target_include_directories(logger_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(logger_static PUBLIC cutils_static)

if(LOGGER_STDOUT_ANSI_COLOR)
    target_compile_definitions(logger_static PUBLIC LOGGER_STDOUT_ANSI_COLOR)
endif()

if(MSVC)
    target_compile_options(logger_static PRIVATE /W4)
else()
    target_compile_options(logger_static PRIVATE -Wall -Wextra -Werror -Wno-format-truncation)
endif()

# ----- installations ----- #

if(LOGGER_INSTALL)
    install(DIRECTORY include DESTINATION .)
    install(TARGETS logger_static DESTINATION lib)
endif()

# ----- tests ----- #

if(LOGGER_BUILD_TESTS)
    add_subdirectory(tests)
endif()
