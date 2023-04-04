cmake_minimum_required(VERSION 3.10)
project(logger-test)

file(GLOB_RECURSE __LOGGER_TESTS_SRC__ *.c)
add_executable(test_logger ${__LOGGER_TESTS_SRC__})
unset(__LOGGER_TESTS_SRC__)

target_link_libraries(test_logger PRIVATE logger_static)

if(MSVC)
    target_compile_options(test_logger PRIVATE /W4)
else()
    target_compile_options(test_logger PRIVATE -Wall -Wextra -Werror)
endif()
