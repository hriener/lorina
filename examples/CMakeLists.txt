# No debugging and optimization for examples
function(add_example name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} lorina)
endfunction()

add_example(pretty_print pretty_print.cpp)
target_link_libraries(pretty_print cli11)
