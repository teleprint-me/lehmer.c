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

set_target_properties(
    lehmer # [<targets>]...
    PROPERTIES # PROPERTIES [<prop1> <value1>]...
    VERSION ${PROJECT_VERSION}
    PUBLIC_HEADER include/lehmer.h
)

target_include_directories(lehmer PUBLIC include)
target_link_libraries(lehmer PUBLIC float_is_close)

# Set the output directory for built binaries
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# Add test executables
add_executable(test_lehmer_period tests/test_lehmer_period.c)
add_executable(test_lehmer_generate tests/test_lehmer_generate.c)
add_executable(test_lehmer_api tests/test_lehmer_api.c)

# Link test executables
target_link_libraries(test_lehmer_period lehmer m)
target_link_libraries(test_lehmer_generate lehmer m)
target_link_libraries(test_lehmer_api lehmer m)

# Set the output directory for the test executables
set_target_properties(
    test_lehmer_generate test_lehmer_api # [<targets>]...
    PROPERTIES # PROPERTIES [<prop1> <value1>]...
    RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
)

# Install rules for the library
install(
    TARGETS lehmer
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

# Add custom targets to run tests
#
# NOTE: Custom targets do not seem to be able to be simplified
# due to the nature of their specificity.
add_custom_target(
    run_test_lehmer_generate
    COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_lehmer_generate
    DEPENDS test_lehmer_generate
    COMMENT "Running tests for lehmer_generate"
)
add_custom_target(
    run_test_lehmer_api
    COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_lehmer_api
    DEPENDS test_lehmer_api
    COMMENT "Running tests for lehmer_api"
)
# NOTE: Slow tests should always be last and testing the period is slow.
add_custom_target(
    run_test_lehmer_period
    COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_lehmer_period
    DEPENDS test_lehmer_period
    COMMENT "Running tests for lehmer_period"
)

enable_testing()
add_test(
    NAME test_lehmer_generate
    COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_lehmer_generate
)
add_test(
    NAME test_lehmer_api
    COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_lehmer_api
)
# NOTE: Slow tests should always be last and testing the period is slow.
add_test(
    NAME test_lehmer_period
    COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_lehmer_period
)
