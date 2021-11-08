BRISTOL parser
==============

The header ``<lorina/bristol.hpp>`` implements methods to parse the BRISTOL formal (see https://homes.esat.kuleuven.be/~nsmart/MPC/).

The class :cpp:class:`lorina::bristol_reader` provides the following public
member functions.

+------------------------------------------+-------------------------------------------------------------------------+
| Function                                 | Description                                                             |
+==========================================+=========================================================================+
| ``on_header(g, w, i, w_p_i, o, w_p_o)``  | Callback method for parsed header                                       |
+------------------------------------------+-------------------------------------------------------------------------+
| ``on_gate(in, out, gate)``               | Callback method for parsed gate                                         |
+------------------------------------------+-------------------------------------------------------------------------+

The following reader functions are available.

.. doc_overview_table:: namespacelorina
   :column: Function

   read_bristol

.. doxygenfunction:: lorina::read_bristol(std::istream&, const bristol_reader&, diagnostic_engine*)
.. doxygenfunction:: lorina::read_bristol(const std::string&, const bristol_reader&, diagnostic_engine*)