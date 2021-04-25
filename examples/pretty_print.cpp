/* lorina: C++ parsing library
 * Copyright (C) 2018  EPFL
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <CLI/CLI.hpp>
#include <lorina/aiger.hpp>
#include <lorina/bench.hpp>
#include <lorina/blif.hpp>
#include <lorina/pla.hpp>
#include <lorina/verilog.hpp>
#include <unordered_map>

using namespace lorina;

enum class format
{
  binary_aiger,
  ascii_aiger,
  bench,
  blif,
  pla,
  verilog,
}; /* format */

std::unordered_map<std::string, format> ext_to_format( {
    {"aig", format::binary_aiger},
    {"aag", format::ascii_aiger},
    {"bench", format::bench},
    {"blif", format::blif},
    {"pla", format::pla},
    {"v", format::verilog},
} );

int main( int argc, char** argv )
{
  std::string filename = "";
  std::string kind_string = "";

  CLI::App app{ "pretty printing" };

  app.add_option( "filename", filename, "file to read" )
    ->required()
    ->check(CLI::ExistingFile);
  app.add_option( "--format", kind_string, "file format" );

  /* parse arguments */
  CLI11_PARSE( app, argc, argv );

  if ( kind_string == "" )
  {
    /* try to auto-detect file-kind by extension */
    const auto pos = filename.find_last_of( "." );
    if ( pos == std::string::npos )
    {
      std::cerr << "[e] could not auto-detect input format by file extension" << std::endl;
      return -1;
    }

    kind_string = filename.substr( pos + 1 );
  }

  const auto it = ext_to_format.find( kind_string );
  if ( it == ext_to_format.end() )
  {
    std::cerr << fmt::format( "[e] unsupported input format `{0}`", kind_string ) << std::endl;
    return -1;
  }

  auto kind = it->second;

  text_diagnostics consumer;
  diagnostic_engine diag( &consumer );
  return_code result;
  switch ( kind )
  {
  case format::ascii_aiger:
    {
      ascii_aiger_pretty_printer reader;
      result = read_ascii_aiger( filename, reader, &diag );
      break;
    }
  case format::binary_aiger:
    {
      ascii_aiger_pretty_printer reader;
      result = read_aiger( filename, reader, &diag );
      break;
    }
  case format::bench:
    {
      bench_pretty_printer reader;
      result = read_bench( filename, reader, &diag );
      break;
    }
  case format::blif:
    {
      blif_pretty_printer reader;
      result = read_blif( filename, reader, &diag );
      break;
    }
  case format::pla:
    {
      pla_pretty_printer reader;
      result = read_pla( filename, reader, &diag );
      break;
    }
  case format::verilog:
    {
      verilog_pretty_printer reader;
      result = read_verilog( filename, reader, &diag );
      break;
    }
  default:
  {
    std::cerr << fmt::format( "[e] no reader specified for `{0}`", kind_string ) << std::endl;
    return -1;
  }
  }

  if ( result == return_code::parse_error )
  {
    std::cerr << "[e] parsing failed" << std::endl;
  }
  else
  {
    std::cerr << "[i] parsing successful" << std::endl;
  }

  return 0;
}
