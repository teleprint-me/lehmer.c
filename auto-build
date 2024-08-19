#!/usr/bin/env bash

# Variables
build_dir="build"
log_file="build.log"
verbose=0
clean=1
log_enabled=0

function print_usage() {
  echo "Usage: ${0} [-v] [-c] [-l]"
  echo -e "\t-v\tEnable verbose output"
  echo -e "\t-c\tDisable cleanup for incremental builds"
  echo -e "\t-l\tEnable logging"
  exit 1
}

# Parse command-line options
while getopts "vcl" option; do
  case $option in
    v) verbose=1 ;;
    c) clean=0 ;;
    l) log_enabled=1 ;;
    *) print_usage ;;
  esac
done

# Enable logging if requested
if [[ $log_enabled -eq 1 ]]; then
  exec > >(tee -i ${log_file})
  exec 2>&1
fi

# Clean the build directory if not disabled and it exists
if [[ $clean -eq 1 && -d $build_dir ]]; then
  echo "Cleaning existing build directory..."
  rm -r "${build_dir}" || { echo "Failed to clean build directory."; exit 1; }
fi

# Generate the build directory with optional verbose output
cmake_options="-B build -DCMAKE_BUILD_TYPE=Debug"
[[ $verbose -eq 1 ]] && cmake_options="$cmake_options -DCMAKE_VERBOSE_MAKEFILE=ON"

# NOTE: shellcheck(SC2086) will complain about word splitting even though 
# that's what we want to happen here. It's best to ignore shellcheck with
# this particular case.
if cmake $cmake_options; then
  echo "CMake configuration successful."
else
  echo "CMake configuration failed."
  exit 1
fi

# Run CMake and build the project
if cmake --build build --config Debug -j "$(nproc)"; then
  echo "Build successful!"
  
  echo "Automating tests..."
  
  # Run tests and handle potential failures
  if ctest --rerun-failed --output-on-failure --test-dir build; then
    echo "Completed testing successfully!"
  else 
    echo "Some tests failed."
    exit 1
  fi
  
  exit 0
else
  echo "Build failed."
  exit 1
fi
