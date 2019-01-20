include_directories(catch2) # v2.0.1

file(GLOB FILENAMES *.cpp)

add_executable(run_tests ${FILENAMES})
if (ENABLE_COVERAGE)
  target_link_libraries(run_tests PUBLIC fmt lorina gcov)
else()
  target_link_libraries(run_tests PUBLIC fmt lorina)
endif()
