BENCH parser
============

The header ``<lorina/bench.hpp>`` implements methods to parse the BENCH format.

The class :cpp:class:`lorina::bench_reader` provides the following public
member functions.

+----------------------------------------+-------------------------------------------------------------------------+
| Function                               | Description                                                             |
+========================================+=========================================================================+
| ``on_input(name)``                     | Callback method for parsed input                                        |
+----------------------------------------+-------------------------------------------------------------------------+
| ``on_output(name)``                    | Callback method for parsed output                                       |
+----------------------------------------+-------------------------------------------------------------------------+
| ``on_gate(inputs, output, type)``      | Callback method for parsed gate                                         |
+----------------------------------------+-------------------------------------------------------------------------+
| ``on_assign(input, output)``           | Callback method for parsed gate assignment                              |
+----------------------------------------+-------------------------------------------------------------------------+

