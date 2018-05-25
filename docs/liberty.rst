LIBERTY parser
==============

The header ``<lorina/liberty.hpp>`` implements methods to parse the LIBERTY format.

The class :cpp:class:`lorina::liberty_reader` provides the following public
member functions.

+----------------------------------------------+-------------------------------------------------------------------------+
| Function                                     | Description                                                             |
+==============================================+=========================================================================+
| ...                                          | Callback method for parsed ...                                          |
+----------------------------------------------+-------------------------------------------------------------------------+

The following reader functions are available.

.. doc_overview_table:: namespacelorina
   :column: Function

   read_liberty

.. doxygenfunction:: lorina::read_liberty(std::istream&, const liberty_reader&, diagnostic_engine *)
.. doxygenfunction:: lorina::read_liberty(const std::string&, const liberty_reader&, diagnostic_engine *)
