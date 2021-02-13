DIMACS parser
=============

The header ``<lorina/dimacs.hpp>`` implements methods to parse the DIMACS CNF/DNF format.

The class :cpp:class:`lorina::dimacs_reader` provides the following public
member functions.

+------------------------------------------+-------------------------------------------------------------------------+
| Function                                 | Description                                                             |
+==========================================+=========================================================================+
| ``on_format(format)``                    | Callback method for parsed format (CNF or DNF)                          |
+------------------------------------------+-------------------------------------------------------------------------+
| ``on_number_of_variables(n)``            | Callback method for parsed number of variables                          |
+------------------------------------------+-------------------------------------------------------------------------+
| ``on_number_of_clauses(n)``              | Callback method for parsed number of clauses                            |
+------------------------------------------+-------------------------------------------------------------------------+
| ``on_clause(clause)``                    | Callback method for parsed clauses                                      |
+------------------------------------------+-------------------------------------------------------------------------+
| ``on_end()``                             | Callback method for parsed end                                          |
+------------------------------------------+-------------------------------------------------------------------------+

The following reader functions are available.

.. doc_overview_table:: namespacelorina
   :column: Function

   read_dimacs


.. doxygenfunction:: lorina::read_dimacs(std::istream&, const dimacs_reader&, diagnostic_engine*)
.. doxygenfunction:: lorina::read_dimacs(const std::string&, const dimacs_reader&, diagnostic_engine*)
