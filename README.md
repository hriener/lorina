[![Build Status](https://travis-ci.org/hriener/lorina.svg?branch=master)](https://travis-ci.org/hriener/lorina)
[![Build status](https://ci.appveyor.com/api/projects/status/vwo8hc9dhtkxhcbl?svg=true)](https://ci.appveyor.com/project/hriener/lorina)
[![Coverage Status](https://coveralls.io/repos/github/hriener/lorina/badge.svg?branch=coverage)](https://coveralls.io/github/hriener/lorina?branch=coverage)
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

class diagnostics : public diagnostic_engine
{
public:
  void emit( diagnostic_level level, const std::string& message ) const override
  {
    /* ... */
  }
}; /* diagnostics */

diagnostics diag;
auto const result = read_bench( "network.bench", reader(), &diag );
assert( result == return_code::success );
```

## EPFL logic sythesis libraries

lorina is part of the [EPFL logic synthesis](https://lsi.epfl.ch/page-138455-en.html) libraries.  The other libraries and several examples on how to use and integrate the libraries can be found in the [logic synthesis tool showcase](https://github.com/lsils/lstools-showcase).
