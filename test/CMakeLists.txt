include_directories(catch2) # v2.0.1

if (UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -Wall -Wextra")
endif()

file(GLOB FILENAMES *.cpp)

add_executable(run_tests ${FILENAMES})
if (ENABLE_COVERAGE)
  target_link_libraries(run_tests PUBLIC fmt lorina gcov)
else()
  target_link_libraries(run_tests PUBLIC fmt lorina)
endif()
