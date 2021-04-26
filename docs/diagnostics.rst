Diagnostics
===========

The header ``<lorina/diagnostics.hpp>`` implements a mechanism to report issues and problems encountered during parsing.

The `diagnostic engine` (class ``diagnostic_engine``) is used to emit diagnostics when a parsing algorithm encounters mistakes.  The possible error messages are specified by the implementation of the respective parsing algorithm.

The `diagnostic builder` (class ``diagnostic_builder``) is a lightweight temporary container for diagnostic information.  A builder is constructed when the method ``report`` of the diagnostic engine is called.  After construction, the diagnostic algorithm may add additional information while the temporary is alive.  When the temporary is destructed, the diagnostic is issued by invoking the method ``emit`` of the diagnostic engine.

The `diagnostic consumer` (class ``diagnostic_consumer``) consumes diagnostics that have been emitted by the diagnostic engine.  The consumer implements a (virtual) method ``handle_diagnostic`` that defines how a diagnostic is processed.  The default consumer ``text_diagnostics`` (derived from ``diagnostic_consumer``) writes the emitted diagnostics to stdout and stderrs, respectively.

**Header:** ``lorina/diagnostics.hpp``

.. toctree::

.. doxygenclass:: lorina::diagnostic_engine
.. doxygenclass:: lorina::diagnostic_builder
.. doxygenclass:: lorina::diagnostic_consumer
.. doxygenclass:: lorina::text_diagnostics
