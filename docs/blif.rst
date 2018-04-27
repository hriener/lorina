BLIF parser
===========

The header ``<lorina/blif.hpp>`` implements methods to parse the BLIF format.

The class :cpp:class:`lorina::blif_reader` provides the following public
member functions.

+----------------------------------------+-------------------------------------------------------------------------+
| Function                               | Description                                                             |
+========================================+=========================================================================+
| ``on_model(model_name)``               | Callback method for parsed model                                        |
+----------------------------------------+-------------------------------------------------------------------------+
| ``on_input(name)``                     | Callback method for parsed input                                        |
+----------------------------------------+-------------------------------------------------------------------------+
| ``on_gate(inputs, output, cover)``     | Callback method for parsed gate                                         |
+----------------------------------------+-------------------------------------------------------------------------+
| ``on_end()``                           | Callback method for parsed end                                          |
+----------------------------------------+-------------------------------------------------------------------------+
| ``on_comment(comment)``                | Callback method for parsed comment                                      |
+----------------------------------------+-------------------------------------------------------------------------+

The following reader functions are available.

.. doc_overview_table:: namespacelorina
   :column: Function

   read_blif

.. doxygenfunction:: lorina::read_blif(std::istream&, const blif_reader&, diagnostic_engine *)
.. doxygenfunction:: lorina::read_blif(const std::string&, const blif_reader&, diagnostic_engine *)
