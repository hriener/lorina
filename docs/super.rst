SUPER parser
============

The header ``<lorina/super.hpp>`` implements methods to parse the SUPER format.

The class :cpp:class:`lorina::super_reader` provides the following public
member functions.

+-------------------------------------------------------------------------+--------------------------------------------+
| Function                                                                | Description                                |
+=========================================================================+============================================+
| ``on_super_info(genlib_name, max_num_vars, max_supergates, num_lines)`` | Callback method for supergate library      |
+-------------------------------------------------------------------------+--------------------------------------------+
| ``on_supergate(name, is_super, fanins_id)``                             | Callback method for parsed gate definition |
+-------------------------------------------------------------------------+--------------------------------------------+

The following reader functions are available.

.. doc_overview_table:: namespacelorina
   :column: Function

   read_super


.. doxygenfunction:: lorina::read_super(std::istream&, const super_reader&, diagnostic_engine*)
.. doxygenfunction:: lorina::read_super(const std::string&, const super_reader&, diagnostic_engine*)
