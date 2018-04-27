AIGER parser
============

The header ``<lorina/aiger.hpp>`` implements methods to parse the AIGER format (see http://fmv.jku.at/aiger/).

The class :cpp:class:`lorina::aiger_reader` provides the following public
member functions.

+------------------------------------------+-------------------------------------------------------------------------+
| Function                                 | Description                                                             |
+==========================================+=========================================================================+
| ``on_header(m, i, l, o, a)``             | Callback method for parsed header                                       |
+------------------------------------------+-------------------------------------------------------------------------+
| ``on_header(m, i, l, o, a, b, c, j, f)`` | Callback method for parsed header                                       |
+------------------------------------------+-------------------------------------------------------------------------+
| ``on_input(index, lit)``                 | Callback method for parsed input                                        |
+------------------------------------------+-------------------------------------------------------------------------+
| ``on_output(index, lit)``                | Callback method for parsed output                                       |
+------------------------------------------+-------------------------------------------------------------------------+
| ``on_latch(index, next, reset)``         | Callback method for parsed latch                                        |
+------------------------------------------+-------------------------------------------------------------------------+
| ``on_and(index, left_lit, right_lit)``   | Callback method for parsed AND gate                                     |
+------------------------------------------+-------------------------------------------------------------------------+
| ``on_input_name(index, name)``           | Callback method for parsed input name                                   |
+------------------------------------------+-------------------------------------------------------------------------+
| ``on_latch_name(index, name)``           | Callback method for parsed latch name                                   |
+------------------------------------------+-------------------------------------------------------------------------+
| ``on_output_name(index, name)``          | Callback method for parsed output name                                  |
+------------------------------------------+-------------------------------------------------------------------------+
| ``on_bad_state_name(index, name)``       | Callback method for a parsed name of a bad state property               |
+------------------------------------------+-------------------------------------------------------------------------+
| ``on_constraint_name(index, name)``      | Callback method for a parsed name of an invariant constraint            |
+------------------------------------------+-------------------------------------------------------------------------+
| ``on_fairness_name(index, name)``        | Callback method for a parsed name of a fairness constraint              |
+------------------------------------------+-------------------------------------------------------------------------+
| ``on_comment(comment)``                  | Callback method for parsed comment                                      |
+------------------------------------------+-------------------------------------------------------------------------+

The following reader functions are available.

.. doc_overview_table:: namespacelorina
   :column: Function

   read_ascii_aiger
   read_aiger

.. doxygenfunction:: lorina::read_ascii_aiger(const std::string&, const aiger_reader&, diagnostic_engine *)
.. doxygenfunction:: lorina::read_ascii_aiger(std::istream&, const aiger_reader&, diagnostic_engine *)
.. doxygenfunction:: lorina::read_aiger(const std::string&, const aiger_reader&, diagnostic_engine *)
.. doxygenfunction:: lorina::read_aiger(std::istream&, const aiger_reader&, diagnostic_engine *)
