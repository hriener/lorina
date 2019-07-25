VERILOG parser
==============

The header ``<lorina/verilog.hpp>`` implements methods to parse a very
simplistic version of the VERILOG format.

The class :cpp:class:`lorina::verilog_reader` provides the following public
member functions.

+-------------------------------------------------------------------+-------------------------------------------------------------------------+
| Function                                                          | Description                                                             |
+===================================================================+=========================================================================+
| ``on_module_header(module_name, inouts)``                         | Callback method for parsed module header                                |
+-------------------------------------------------------------------+-------------------------------------------------------------------------+
| ``on_inputs(inputs,size)``                                        | Callback method for parsed input declarations                           |
+-------------------------------------------------------------------+-------------------------------------------------------------------------+
| ``on_outputs(outputs,size)``                                      | Callback method for parsed output declarations                          |
+-------------------------------------------------------------------+-------------------------------------------------------------------------+
| ``on_wires(wires,size)``                                          | Callback method for parsed wire declarations                            |
+-------------------------------------------------------------------+-------------------------------------------------------------------------+
| ``on_parameter(name, value)``                                     | Callback method for parsed parameter definitions                        |
+-------------------------------------------------------------------+-------------------------------------------------------------------------+
| ``on_module_instantiation(module_name, params, inst_name, args)`` | Callback method for parsed immediate assignment                         |
+-------------------------------------------------------------------+-------------------------------------------------------------------------+
| ``on_assign(lhs, rhs)``                                           | Callback method for parsed immediate assignment                         |
+-------------------------------------------------------------------+-------------------------------------------------------------------------+
| ``on_and(lhs, op1, op2)``                                         | Callback method for parsed AND assignment (with 2 operands)             |
+-------------------------------------------------------------------+-------------------------------------------------------------------------+
| ``on_nand(lhs, op1, op2)``                                        | Callback method for parsed NAND assignment (with 2 operands)            |
+-------------------------------------------------------------------+-------------------------------------------------------------------------+
| ``on_or(lhs, op1, op2)``                                          | Callback method for parsed OR assignment (with 2 operands)              |
+-------------------------------------------------------------------+-------------------------------------------------------------------------+
| ``on_nor(lhs, op1, op2)``                                         | Callback method for parsed NOR assignment (with 2 operands)             |
+-------------------------------------------------------------------+-------------------------------------------------------------------------+
| ``on_xor(lhs, op1, op2)``                                         | Callback method for parsed XOR assignment (with 2 operands)             |
+-------------------------------------------------------------------+-------------------------------------------------------------------------+
| ``on_xnor(lhs, op1, op2)``                                        | Callback method for parsed XNOR assignment (with 2 operands)            |
+-------------------------------------------------------------------+-------------------------------------------------------------------------+
| ``on_and3(lhs, op1, op2, op3)``                                   | Callback method for parsed AND assignment (with 3 operands)             |
+-------------------------------------------------------------------+-------------------------------------------------------------------------+
| ``on_or3(lhs, op1, op2, op3)``                                    | Callback method for parsed OR assignment (with 3 operands)              |
+-------------------------------------------------------------------+-------------------------------------------------------------------------+
| ``on_xor3(lhs, op1, op2, op3)``                                   | Callback method for parsed XOR assignment (with 3 operands)             |
+-------------------------------------------------------------------+-------------------------------------------------------------------------+
| ``on_maj3(lhs, op1, op2, op3)``                                   | Callback method for parsed MAJ3 assignment                              |
+-------------------------------------------------------------------+-------------------------------------------------------------------------+
| ``on_endmodule``                                                  | Callback method for parsed end of module                                |
+-------------------------------------------------------------------+-------------------------------------------------------------------------+

The following reader functions are available.

.. doc_overview_table:: namespacelorina
   :column: Function

   read_verilog

.. doxygenfunction:: lorina::read_verilog(const std::string&, const verilog_reader&, diagnostic_engine *)
.. doxygenfunction:: lorina::read_verilog(std::istream&, const verilog_reader&, diagnostic_engine *)
