add_library(lorina_fmt INTERFACE)
target_include_directories(lorina_fmt INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/fmt)
target_compile_definitions(lorina_fmt INTERFACE FMT_HEADER_ONLY)

add_library(lorina_cli11 INTERFACE)
target_include_directories(lorina_cli11 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/cli11)
