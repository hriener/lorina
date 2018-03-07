Installation
============

lorina is a header-only C++ library.  Just add the include directory
of lorina to your include directories, and you can integrate lorina into
your source files using

.. code-block:: c++

   #include <lorina/lorina.hpp>

Building examples
-----------------

Examples are enabled by default.  In order to build them, run the
following commands from the base directory of lorina::

  mkdir build
  cd build
  cmake ..
  make

Building tests
--------------

In order to run the tests, you need to enable them in CMake::

  mkdir build
  cd build
  cmake -DLORINA_TEST=ON ..
  make
  ./test/run_tests
