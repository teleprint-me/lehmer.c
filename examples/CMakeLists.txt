# Lehmer LCG RNG Examples CMakeLists.txt

# Add lehmer related examples
set(EXAMPLES 
    algorithm
    binder
    concept
    generator
    modulus
    validator
)

# Create executables for simple lehmer examples
# Loop over each example and create an executable
foreach(example IN LISTS EXAMPLES)
    add_executable(${example} ${PROJECT_SOURCE_DIR}/examples/${example}.c)
    target_link_libraries(${example} lehmer)
    target_include_directories(${example} PRIVATE ${PROJECT_SOURCE_DIR}/include)
    set_target_properties(${example} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/examples)
endforeach()
