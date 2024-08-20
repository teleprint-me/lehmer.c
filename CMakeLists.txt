cmake_minimum_required(VERSION 3.14)
project(lehmer VERSION 0.1.0 DESCRIPTION "ANSI C library for Random Number Generation")

# Set C standard rules
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# Add subdirectory for the float_is_close dependency
add_subdirectory(float_is_close)

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
target_link_libraries(lehmer PUBLIC float_is_close)

# Set the output directory for built binaries
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# Add test executable
add_executable(test_lehmer tests/test_lehmer_api.c)
target_link_libraries(test_lehmer lehmer m)

# Set the output directory for the test executable
set_target_properties(test_lehmer PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# Install rules for the library
install(
    TARGETS lehmer
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

# Add a custom target to run tests
add_custom_target(
    run_test_lehmer
    COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_lehmer
    DEPENDS test_lehmer
    COMMENT "Running tests for lehmer"
)

enable_testing()
add_test(NAME test_lehmer COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_lehmer)
