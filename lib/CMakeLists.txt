if (NOT TARGET cli11)
  add_library(cli11 INTERFACE)
  target_include_directories(cli11 SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/cli11)
endif()

if (NOT TARGET fmt)
  add_library(fmt INTERFACE)
  target_include_directories(fmt SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/fmt)
  target_compile_definitions(fmt INTERFACE FMT_HEADER_ONLY)
endif()

if (NOT TARGET json)
  add_library(json INTERFACE) # v3.5.0
  target_include_directories(json SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/json)
endif()

if (NOT TARGET infint)
  add_library(infint INTERFACE)
  target_include_directories(infint SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/infint)
endif()
