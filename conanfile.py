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

    # Remove manually included deps
    tools.replace_in_file(
      "CMakeLists.txt",
      "project(lorina LANGUAGES CXX)",
      '''\nproject(lorina LANGUAGES CXX)\n'''
      '''include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)\n'''
      '''conan_basic_setup()''',
      strict=True
    )

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

  def package(self):
    cmake = CMake(self)
    cmake.configure()
    cmake.install()
    self.copy("*.hpp", dst="include", src="include")
    self.copy("LICENSE", ignore_case=True, keep_path=False)

  def package_id(self):
    self.info.header_only()
