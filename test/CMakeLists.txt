include_directories(catch2) # v2.0.1

file(GLOB FILENAMES *.cpp)

add_executable(run_tests ${FILENAMES})
if (ENABLE_COVERAGE)
  target_link_libraries(run_tests PUBLIC fmt json lorina gcov)
else()
  target_link_libraries(run_tests PUBLIC fmt json lorina)
endif()

# path to the benchmark files
target_compile_definitions(run_tests PUBLIC "BENCHMARKS_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/../\"")
