cmake_minimum_required(VERSION 3.1)
project(lorina LANGUAGES CXX)

# Options
option(LORINA_EXAMPLES "Build examples" ON)
option(LORINA_TEST "Build tests" OFF)

if (CMAKE_COMPILER_IS_GNUCC)
  option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" OFF)

  if (ENABLE_COVERAGE)
    add_compile_options(-O0 -g --coverage -fprofile-arcs -ftest-coverage)
  endif()
endif()

# some specific compiler definitions
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fcolor-diagnostics" HAS_FCOLOR_DIAGNOSTICS)
if (HAS_FCOLOR_DIAGNOSTICS)
  add_definitions(-fcolor-diagnostics)
endif()

if (UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
endif()

add_subdirectory(lib)
add_subdirectory(include)

if(LORINA_EXAMPLES)
  add_subdirectory(examples)
endif()

if(LORINA_TEST)
  add_subdirectory(test)
endif()
