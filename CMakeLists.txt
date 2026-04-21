cmake_minimum_required(VERSION 3.14)
project(logger)

# ----- options ----- #

option(LOGGER_BUILD_TESTS "build tests" ON)
option(LOGGER_INSTALL "install headers and libs" ON)
option(LOGGER_STDOUT_ANSI_COLOR "use ansi color in stdout" ON)

# ----- targets ----- #

file(GLOB __LOGGER_SRC__ ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
add_library(logger_static STATIC ${__LOGGER_SRC__})
unset(__LOGGER_SRC__)

target_include_directories(logger_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(LOGGER_STDOUT_ANSI_COLOR)
    target_compile_definitions(logger_static PUBLIC LOGGER_STDOUT_ANSI_COLOR)
endif()

if(MSVC)
    target_compile_definitions(logger_static PRIVATE _CRT_SECURE_NO_WARNINGS)
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
