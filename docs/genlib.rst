GENLIB parser
=============

The header ``<lorina/genlib.hpp>`` implements methods to parse the GENLIB format.

The class :cpp:class:`lorina::genlib_reader` provides the following public
member functions.

+-------------------------------------------------------+--------------------------------------------+
| Function                                              | Description                                |
+=======================================================+============================================+
| ``on_gate(name, expression, area, pins, output_pin)`` | Callback method for parsed gate definition |
+-------------------------------------------------------+--------------------------------------------+

The following reader functions are available.

.. doc_overview_table:: namespacelorina
   :column: Function

   read_genlib


.. doxygenfunction:: lorina::read_genlib(std::istream&, const genlib_reader&, diagnostic_engine*)
.. doxygenfunction:: lorina::read_genlib(const std::string&, const genlib_reader&, diagnostic_engine*)
