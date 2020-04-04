Change Log
==========

v0.3 (Not yet released)
-----------------------

* Bristol fashion reader: ``bristol_reader`` `#32 <https://github.com/hriener/lorina/pull/32>`_
* BLIF reader: ``on_latch`` (contributed by Max Austin) `#28 <https://github.com/hriener/lorina/pull/28>`_
* Writer visitor: ``verilog_writer``
* Verilog reader: ``on_module_instantiation``, ``on_parameter``
* Verilog reader: ``on_inputs``, ``on_outputs``, ``on_wires``
* Verilog reader: ``on_nand``, ``on_nor``, ``on_xnor``
* Bench reader: ``on_dff_input``, ``on_dff``

v0.2 (October 18, 2018)
-----------------------

* Reader visitor: ``verilog_reader``
* Pretty printer: ``verilog_pretty_printer``
* Writer visitor: ``pla_writer``
* New AIGER 1.9 parser: ``aiger_reader``, ``ascii_aiger_pretty_printer``

v0.1 (April 27, 2018)
---------------------

* Reader visitors: ``aiger_reader``, ``bench_reader``, ``blif_reader``, ``pla_reader``
* Pretty printer: ``ascii_aiger_pretty_printer``, ``bench_pretty_priter``, ``blif_pretty_printer``, ``pla_pretty_printer``
* Diagnostic infrastructure: ``diagnostic_builder``, ``diagnostic_engine``, ``silent_diagnostic_engine``
* Generic topological re-sorting: ``call_in_topological_order``
