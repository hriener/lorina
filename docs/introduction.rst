Introduction
============

The C++ library `lorina` implements parsers for simple file formats
used in logic synthesis and formal verification.  A callback mechanism
in form of overloadable visitor classes allows users to react on
parsed primitives and parse errors.

The library consists of several independent parsers.  Each parser has
its own header file that contains a `reader function`
``read_<format>`` and a `reader visitor` ``<format>_reader``, where
``<format>`` has to be substituted with the format to be parsed.  The
reader visitor class ``<format>_reader`` provides a set of default
implementations of virtual ``on_<primitive>`` methods called by the
parser.  Each callback method corresponds to an `event point` defined
by the implementation of the parsing algorithm, e.g., the completion
of the parsing of the format’s header information, or a certain input
or gate definition.

The ``read_<format>`` function can either be used to open and parse a
file provided by name (``const std::string& filename``) or to parse
tokens from an input stream (``std::istream& in``)::

  /* read file */
  const std::string filename = ...;
  read_<format>( filename, <format>_reader() );

  /* read istream */
  std::ifstream in( filename.c_str(), std::ifstream::in );
  read_<format>( in, <format>_reader() );

The default behavior of any parser can be customized by deriving a new
class from a reader visitor and overloading its virtual callback
methods.
