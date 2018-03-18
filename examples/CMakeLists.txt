# No debugging and optimization for examples
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -DNDEBUG")

function(add_example name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} lorina)
endfunction()

add_example(pretty_print pretty_print.cpp)
target_link_libraries(pretty_print lorina_cli11)
