cmake_minimum_required(VERSION 3.14)
project(lehmer VERSION 0.1.0 DESCRIPTION "ANSI C library for Random Number Generation")

# Set C standard rules
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# Main library
add_library(
    lehmer
    src/lehmer.c
)

set_target_properties(lehmer PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(
    lehmer PROPERTIES 
    PUBLIC_HEADER include/lehmer.h
)

target_include_directories(lehmer PUBLIC include)

# Set the output directory for built binaries
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# Add test executable
add_executable(test_float_is_close tests/test_is_float_close.c)
target_link_libraries(test_float_is_close lehmer m)

# Set the output directory for the test executable
set_target_properties(test_float_is_close PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# Install rules for the library
install(
    TARGETS lehmer
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

# Add a custom target to run tests
add_custom_target(
    run_tests
    COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_float_is_close
    DEPENDS test_float_is_close
    COMMENT "Running tests for float_is_close"
)

enable_testing()
add_test(NAME float_is_close COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_float_is_close)
