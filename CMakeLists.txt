cmake_minimum_required(VERSION 3.1)
project(lorina LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(LORINA_EXAMPLES "Build examples" ON)
option(LORINA_TEST "Build tests" OFF)

# some specific compiler definitions
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fcolor-diagnostics" HAS_FCOLOR_DIAGNOSTICS)
if (HAS_FCOLOR_DIAGNOSTICS)
  add_definitions(-fcolor-diagnostics)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

add_subdirectory(lib)
add_subdirectory(include)

if(LORINA_EXAMPLES)
  add_subdirectory(examples)
endif()

if(LORINA_TEST)
  add_subdirectory(test)
endif()
