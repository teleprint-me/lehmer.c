cmake_minimum_required(VERSION 3.14)
project(lehmer VERSION 0.1.0 DESCRIPTION "ANSI C library for Number Generation")

# Set C standard rules
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Enable Shared Objects
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# Setup submodule, module, and test names
set(SUBMODULES ccheck) # logger and float_is_close are implied
set(MODULES lehmer prime) # Add more modules here as needed

# Set the output directory for built binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add dependencies
foreach(SUBMODULE IN LISTS SUBMODULES)
    add_subdirectory(mods/${SUBMODULE})
endforeach()

# Collect all source files into lists
foreach(MODULE IN LISTS MODULES)
    list(APPEND LIB_HEADERS "include/${MODULE}.h")
    list(APPEND LIB_SOURCES "src/${MODULE}.c")
endforeach()

# Add the library using the collected sources
add_library(lehmer ${LIB_SOURCES})

set_target_properties(
    lehmer PROPERTIES
    VERSION ${PROJECT_VERSION}
    PUBLIC_HEADER "${LIB_HEADERS}"
)

target_include_directories(lehmer PUBLIC include)
target_link_libraries(lehmer PUBLIC m rt logger float_is_close ccheck)

# Add and link test executables
foreach(MODULE IN LISTS MODULES)
    add_executable("test_${MODULE}" "tests/test_${MODULE}.c")
    target_link_libraries("test_${MODULE}" PRIVATE lehmer m)
    set_target_properties("test_${MODULE}" PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
    add_test(NAME "test_${MODULE}" COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_${MODULE}")
endforeach()

# Install rules for the library
install(
    TARGETS lehmer
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

# Enable testing
enable_testing()
