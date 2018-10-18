Change Log
==========

v0.3 (Not yet released)
-----------------------

v0.2 (October 18, 2018)
-----------------------

* Reader visitor for a simplistic version of structural Verilog (`verilog_reader`)
* Implementation of a pretty-printer for Verilog: (`verilog_pretty_printer`)
* Writer visitor for assembling strings to PLA (`pla_writer`)
* Support for parsing AIGER 1.9 (`aiger_reader`, `ascii_aiger_pretty_printer`)

v0.1 (April 27, 2018)
---------------------

* Reader visitors for simple file formats commonly used in logic synthesis and formal verifiation (`aiger_reader`, `bench_reader`, `blif_reader`, `pla_reader`)
* Implementations of reader visitors for pretty-printing (`ascii_aiger_pretty_printer`, `bench_pretty_priter`, `blif_pretty_printer`, `pla_pretty_printer`)
* Diagnostic infrastructure (`diagnostic_builder``, `diagnostic_engine`, `silent_diagnostic_engine`)
* Utilities for generic topological re-sorting (`call_in_topological_order`)
