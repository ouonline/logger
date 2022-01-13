cmake_minimum_required(VERSION 3.10)
project(logger-test)

add_executable(test_logger test_logger.c)
target_link_libraries(test_logger PRIVATE logger_static)
