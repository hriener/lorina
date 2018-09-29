Diagnostics
===========

The header ``<lorina/diagnostics.hpp>`` implements a mechanism to react on parse errors.

The `diagnostic engine` (class ``diagnostic_engine``) is used to emit diagnostics when a parsing algorithm encounters mistakes.  The possible error messages are specified by the implementation of the respective parsing algorithm.

The `diagnostic builder` (class ``diagnostic_builder``) is a lightweight temporary container for diagnostic information.  A builder is constructed when the method ``report`` of the diagnostic engine is called.  After construction, the diagnostic algorithm may add additional information while the temporary is alive.  When the temporary is destructed, the diagnostic is issued by invoking the method ``emit`` of the diagnostic engine.

**Header:** ``lorina/diagnostics.hpp``

.. toctree::

.. doxygenclass:: lorina::diagnostic_builder
.. doxygenclass:: lorina::diagnostic_engine
.. doxygenclass:: lorina::silent_diagnostic_engine
