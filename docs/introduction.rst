Introduction
============

The C++ library `lorina` implements parsers for simple file formats
used in logic synthesis and formal verification.  A callback mechanism
in form of overloadable visitor classes allows users to react on
parsed primitives and parse errors.

The library consists of several independent parsers.  Each parser has
its own header file that contains a function `read_<format>` and a
visitor class `<format>_reader` to be overloaded, where `<format>` has
to be substituted with the format to be parsed.  The visitor class
`<format>_reader` provides a set of default implementations of virtual
`on_<primitive>` methods called by the parser whenever a primitive in
the respective format is recognized.  The behavior of the parser can
be customized by overriden the `on_<primitive>` methods.

The `read_<format>` function can either be used to open and parse a
file provided by name (`const std::string& filename`) or to parse
tokens from an input stream (`std::istream& in`).
C++::
  const std::string filename = ...;
  read_<format>( filename, read_<format>() );

C++::
  std::ifstream in( filename.c_str(), std::ifstream::in );
  read_<format>( in, read_<format>() );
