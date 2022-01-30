cmake_minimum_required(VERSION 3.10)
project(logger)

# ----- options ----- #

option(LOGGER_BUILD_TESTS "build tests" ON)
option(LOGGER_INSTALL "install headers and libs" ON)

# ----- targets ----- #

file(GLOB LOGGER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
add_library(logger_static STATIC ${LOGGER_SRC})
target_include_directories(logger_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(logger_static PUBLIC pthread)

# ----- dependencies ----- #

if(NOT cutils_static)
    set(CUTILS_BUILD_TESTS OFF CACHE BOOL "")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../cutils ${CMAKE_CURRENT_BINARY_DIR}/cutils)
    target_link_libraries(logger_static PUBLIC cutils_static)
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
