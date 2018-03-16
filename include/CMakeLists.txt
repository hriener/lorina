add_library(lorina INTERFACE)
target_include_directories(lorina INTERFACE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(lorina INTERFACE lorina_fmt lorina_rang)
