cmake_minimum_required(VERSION 3.1)
project(lorina LANGUAGES CXX)

# Options
option(LORINA_EXAMPLES "Build examples" ON)
option(LORINA_TEST "Build tests" OFF)
option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" OFF)

if (UNIX)
  # show quite some warnings (but remove some intentionally)
  include(CheckCXXCompilerFlag)
  add_compile_options(-Wall -Wextra -pedantic)
  foreach (WARNING unknown-pragmas)
    check_cxx_compiler_flag("-Wno-${WARNING}" HAS_WNO_${WARNING})
    if (HAS_WNO_${WARNING})
      add_compile_options(-Wno-${WARNING})
    endif()
  endforeach()
  if (ENABLE_COVERAGE)
    add_compile_options(-O0 -g --coverage -fprofile-arcs -ftest-coverage)
  endif()
endif()

add_subdirectory(lib)
add_subdirectory(include)

if(LORINA_EXAMPLES)
  add_subdirectory(examples)
endif()

if(LORINA_TEST)
  add_subdirectory(test)
endif()
