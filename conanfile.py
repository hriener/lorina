# vim: set ts=2 sw=2 tw=0 et :

from conans import ConanFile, CMake, tools


class LorinaConan(ConanFile):
  name = "lorina"
  version = "0.2"
  license = "MIT"
  author = "Heinz Riener"
  url = "https://github.com/hriener/lorina"
  description = "C++ parsing library for simple formats used in logic synthesis and formal verification "
  topics = ("parsing", "logic-synthesis", "verification", "epfl")
  generators = "cmake"
  exports_sources = "include/*", "cmake/*", "LICENSE", "CMakeLists.txt"
  requires = "fmt/6.1.2", "rang/3.1.0@rang/stable"
  no_copy_source = False

  def source(self):
    self.run("git clone git@github.com:hriener/lorina.git")

    # Remove manually inserted libraries
    tools.replace_in_file(
      "CMakeLists.txt",
      "add_subdirectory(lib)",
      "",
      strict=True
    )

    # Remove examples
    tools.replace_in_file(
      "CMakeLists.txt",
      "add_subdirectory(examples)",
      '',
      strict=True
    )

    # Correct rang header
    tools.replace_in_file(
      "include/lorina/diagnostics.hpp",
      "#include <rang/rang.hpp>",
      '#include <rang.hpp>',
      strict=True
    )
    # Create install target
    tools.replace_in_file(
      "CMakeLists.txt",
      '''if(LORINA_TEST)\n'''
      '''  add_subdirectory(test)\n'''
      '''endif()\n''',
      '''# Install\n'''
      '''include(GNUInstallDirs)\n\n'''
      '''# Export targets\n'''
      '''install(\n'''
      '''  TARGETS lorina\n'''
      '''  EXPORT lorina-targets\n'''
      '''  DESTINATION ${CMAKE_INSTALL_LIBDIR}\n'''
      ''')\n'''
      '''# Install exported targets\n'''
      '''install(EXPORT lorina-targets\n'''
      '''  FILE lorina-targets.cmake\n'''
      '''  NAMESPACE lorina::\n'''
      '''  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/\n'''
      ''')\n'''
      '''# Install lorina-config.cmake\n'''
      '''install(\n'''
      '''  FILES\n'''
      '''    ${CMAKE_SOURCE_DIR}/cmake/lorina-config.cmake\n'''
      '''  DESTINATION\n'''
      '''    ${CMAKE_INSTALL_LIBDIR}/cmake/\n'''
      ''')\n'''
      '''# Install headers\n'''
      '''install(\n'''
      '''  DIRECTORY\n'''
      '''    ${PROJECT_SOURCE_DIR}/include/lorina\n'''
      '''  DESTINATION\n'''
      '''    ${CMAKE_INSTALL_PREFIX}/include\n'''
      ''')\n'''
      '''# Install headers\n'''
      '''install(\n'''
      '''  DIRECTORY\n'''
      '''    ${PROJECT_SOURCE_DIR}/include/lorina\n'''
      '''  DESTINATION\n'''
      '''    ${CMAKE_INSTALL_PREFIX}/include\n'''
      ''')\n''',
      strict=True
    )

  def package(self):
    cmake = CMake(self)
    cmake.configure()
    cmake.install()
    self.copy("*.hpp", dst="include", src="include")
    self.copy("LICENSE", ignore_case=True, keep_path=False)

  def package_id(self):
    self.info.header_only()
