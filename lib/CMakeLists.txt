if (NOT TARGET cli11)
  add_library(cli11 INTERFACE)
  target_include_directories(cli11 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/cli11)
endif()

if (NOT TARGET fmt)
  add_library(fmt INTERFACE)
  target_include_directories(fmt INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/fmt)
  target_compile_definitions(fmt INTERFACE FMT_HEADER_ONLY)
endif()

if (NOT TARGET rang)
  add_library(rang INTERFACE)
  target_include_directories(rang INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/rang)
endif()

if (NOT TARGET json)
  add_library(json INTERFACE) # v3.5.0
  target_include_directories(json INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/json)
endif()
