[![Actions Status](https://github.com/hriener/lorina/workflows/Linux%20CI/badge.svg)](https://github.com/hriener/lorina/actions)
[![Actions Status](https://github.com/hriener/lorina/workflows/MacOS%20CI/badge.svg)](https://github.com/hriener/lorina/actions)
[![Actions Status](https://github.com/hriener/lorina/workflows/Windows%20CI/badge.svg)](https://github.com/hriener/lorina/actions)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/a00baa65ac514b17b7594b49d2058a0c)](https://www.codacy.com/gh/hriener/lorina/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=hriener/lorina&amp;utm_campaign=Badge_Grade)
[![Coverage Status](https://codecov.io/gh/hriener/lorina/branch/master/graph/badge.svg?token=ZAWW94ECYU)](https://codecov.io/gh/hriener/lorina)
[![Documentation Status](https://readthedocs.org/projects/lorina/badge/?version=latest)](http://lorina.readthedocs.io/en/latest/?badge=latest)
[![Latest release](https://img.shields.io/github/release/hriener/lorina.svg)](https://github.com/hriener/lorina/releases)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# lorina

<img src="https://cdn.rawgit.com/hriener/lorina/master/lorina.svg" width="64" height="64" align="left" style="margin-right: 12pt" />
lorina is a C++ parsing library.  It provides implementations of parsers for various simple file formats used in logic synthesis and formal verification.

[Read the full documentation.](http://lorina.readthedocs.io/en/latest/?badge=latest)

## Example

The following code snippet parses the bench file `"network.bench"` and calls the methods `on_input` and `on_gate` for each input and gate declaration in the file, respectively.

```c++
#include <lorina/lorina.hpp>

class reader : public bench_reader
{
public:
  void on_input( const std::string& name ) const override
  {
    /* ... */
  }

  void on_gate( const std::vector<std::string>& inputs, const std::string& output,
                const std::string& type ) const override
  {
    /* ... */
  }
}; /* reader */

auto const result = read_bench( "network.bench", reader() );
assert( result == return_code::success );
```

Besides parsing, the reader supports a mechanism to react on parse errors.

```c++
#include <lorina/diagnostics.hpp>

class diagnostics : public diagnostic_consumer
{
public:
  void handle_diagnostic( diagnostic_level level, std::string const& message ) const override
  {
    /* ... */
  }
}; /* diagnostics */

diagnostics consumer;
diagnostic_engine diag( &consumer );
auto const result = read_bench( "network.bench", reader(), &diag );
assert( result == return_code::success );
```

## EPFL logic sythesis libraries

lorina is part of the [EPFL logic synthesis](https://lsi.epfl.ch/page-138455-en.html) libraries.  The other libraries and several examples on how to use and integrate the libraries can be found in the [logic synthesis tool showcase](https://github.com/lsils/lstools-showcase).

