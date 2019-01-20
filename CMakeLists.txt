cmake_minimum_required(VERSION 3.1)
project(lorina LANGUAGES CXX)

# Options
option(LORINA_EXAMPLES "Build examples" ON)
option(LORINA_TEST "Build tests" OFF)
option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" OFF)

add_compile_options(-Wall -Wextra -pedantic)
if (ENABLE_COVERAGE)
  Add_compile_options(-O0 -g --coverage -fprofile-arcs -ftest-coverage)
endif()

# some specific compiler definitions
# include(CheckCXXCompilerFlag)
# check_cxx_compiler_flag("-fcolor-diagnostics" HAS_FCOLOR_DIAGNOSTICS)
# if (HAS_FCOLOR_DIAGNOSTICS)
#   add_definitions(-fcolor-diagnostics)
# endif()

add_subdirectory(lib)
add_subdirectory(include)

if(LORINA_EXAMPLES)
  add_subdirectory(examples)
endif()

if(LORINA_TEST)
  add_subdirectory(test)
endif()
