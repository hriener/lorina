# Find dependencies
include(CMakeFindDependencyMacro)
find_dependency(fmt)
find_dependency(rang)

# Make targets available
include("${CMAKE_CURRENT_LIST_DIR}/lorina-targets.cmake")
