cmake_minimum_required(VERSION 3.13)
project(xd LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Find LLVM ---
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# --- Include LLVM directories and definitions ---
include_directories(${LLVM_INCLUDE_DIRS} include)
add_definitions(${LLVM_DEFINITIONS})

# --- Add executable ---
add_executable(xd
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/generator.cpp
)

# --- Automatically detect and link all required LLVM components ---
# Option 1: Explicit (recommended for control)
llvm_map_components_to_libnames(LLVM_LIBS
    Core
    Support
    IRReader
    ExecutionEngine
    MC
    MCJIT
    native
)

# Option 2 (uncomment this if you want CMake to auto-link *everything*):
# set(LLVM_LIBS ${LLVM_AVAILABLE_LIBS})

# --- Link target with LLVM ---
target_link_libraries(xd PRIVATE ${LLVM_LIBS})

# --- Optional: add LLVM compile flags ---
target_compile_options(xd PRIVATE ${LLVM_COMPILE_FLAGS})

