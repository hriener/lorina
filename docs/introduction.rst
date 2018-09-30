Introduction
============

Synopsis
--------

The C++ library `lorina` implements parsers for simple file formats
used in logic synthesis and formal verification.  A callback mechanism
in form of customizable visitor classes allows users to react on
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

Customization
-------------

The default behavior of any reader visitor can be customized by
deriving from a reader visitor and overriding its virtual callback
methods.  In the following code snippet, the class
``print_bench_input_decl`` derives from ``bench_reader`` and
customizes the behavior of the ``on_input`` event point::

  class print_bench_input_decl : public bench_reader
  {
  public:
    void on_input( const std::string& name ) const override
    {
      std::cout << "INPUT: " << name << std::endl;
    }
  };

Diagnostics
-----------

To reduce code and executable size, error handling is implemented via
return codes and a lightweight callback visitor mechanism (instead of
C++ exceptions).  This allows users to embed lorina into their
projects even when C++ exceptions are globally disabled.

All reader functions ``read_<format>`` either return
``return_code::success`` if parsing has been successful or otherwise
``return_code::parse_error``.  The `diagnostic engine` additionally
allows users to react on parse errors.  Diagnostics are automatically
activated by providing a pointer to a ``diagnostic_engine`` as a third
parameter to a reader function::

  diagnostic_engine diag;
  return_code result = read_<format>( filename, <format>_reader(), &diag );

The `parse error` event points and the corresponding `error messages`
are specified by the respective parsing algorithm.  The default
behavior of the diagnostic engine can be customized (similarly to the
reader visitors) by deriving from `diagnostic_engine` and overriding
its virtual callback method ``emit``.
