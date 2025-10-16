cmake_minimum_required(VERSION 3.13)
project(XD CXX C)

set(CMAKE_CXX_STANDARD 20)
# Find the LLVM package
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Include LLVM directories
include_directories(${LLVM_INCLUDE_DIRS} include)

# Add definitions from LLVM
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Add executable
# Find LLVM package
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Store LLVM include directories in cache
set(LLVM_INCLUDE_DIRS ${LLVM_INCLUDE_DIRS} CACHE STRING "LLVM include directories")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

add_executable(xd src/main.cpp src/lexer.cpp src/parser.cpp src/generator.cpp)

# Map LLVM components to libraries
llvm_map_components_to_libnames(llvm_libs support core irreader)

# Link against LLVM libraries
target_link_libraries(xd ${llvm_libs})

# Manually specify compiler and linker flags
set(LLVM_LD_FLAGS "-L${LLVM_LIBRARY_DIRS} -lLLVM")

# Add compiler flags and linker flags manually
target_compile_options(xd PRIVATE ${LLVM_CXX_FLAGS})
target_link_libraries(xd ${LLVM_LD_FLAGS})

