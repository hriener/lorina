add_library(lorina INTERFACE)
target_include_directories(lorina
    INTERFACE
      $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
if (ENABLE_COVERAGE)
  target_link_libraries(lorina INTERFACE fmt rang gcov)
else()
  target_link_libraries(lorina INTERFACE fmt rang)
endif()
