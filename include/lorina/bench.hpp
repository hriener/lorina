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

/*!
  \file bench.hpp
  \brief Implements bench parser

  \author Heinz Riener
*/

#pragma once

#include <lorina/common.hpp>
#include <lorina/diagnostics.hpp>
#include <lorina/detail/utils.hpp>
#include <fstream>
#include <regex>
#include <iostream>

namespace lorina
{

class bench_reader
{
public:
  virtual void on_input( const std::string& name ) const
  {
    (void)name;
  }

  virtual void on_output( const std::string& name ) const
  {
    (void)name;
  }

  virtual void on_gate( const std::vector<std::string>& inputs, const std::string& output, const std::string& type ) const
  {
    (void)inputs;
    (void)output;
    (void)type;
  }

  virtual void on_assign( const std::string& input, const std::string& output ) const
  {
    (void)input;
    (void)output;
  }
}; /* bench_reader */

namespace bench_regex
{
static std::regex input( R"(INPUT\((.*)\))" );
static std::regex output( R"(OUTPUT\((.*)\))" );
static std::regex gate( R"((.*)\s+=\s+(.*)\((.*)\))" );
static std::regex lut( R"((.*)\s+=\s+LUT\s+(.*)\((.*)\))" );
static std::regex gate_asgn( R"((.*)\s+=\s+(.*))" );
} // namespace bench_regex

inline return_code read_bench( std::istream& in, const bench_reader& reader, diagnostic_engine* diag = nullptr )
{
  return_code result = return_code::success;

  std::smatch m;
  detail::foreach_line_in_file_escape( in, [&]( const std::string& line ) {
    /* empty line or comment */
    if ( line.empty() || line[0] == '#' )
      return true;

    /* INPUT(<string>) */
    if ( std::regex_search( line, m, bench_regex::input ) )
    {
      reader.on_input( detail::trim_copy( m[1] ) );
      return true;
    }

    /* OUTPUT(<string>) */
    if ( std::regex_search( line, m, bench_regex::output ) )
    {
      reader.on_output( detail::trim_copy( m[1] ) );
      return true;
    }

    /* (<string> = LUT <HEX>(<list of strings>)) */
    if ( std::regex_search( line, m, bench_regex::lut ) )
    {
      const auto output = detail::trim_copy( m[1] );
      const auto type = detail::trim_copy( m[2] );
      const auto args = detail::trim_copy( m[3] );
      const auto inputs = detail::split( args, "," );
      reader.on_gate( inputs, output, type );
      return true;
    }

    /* (<string> = <GATE_TYPE>(<list of strings>)) */
    if ( std::regex_search( line, m, bench_regex::gate ) )
    {
      const auto output = detail::trim_copy( m[1] );
      const auto type = detail::trim_copy( m[2] );
      const auto args = detail::trim_copy( m[3] );
      const auto inputs = detail::split( args, "," );
      reader.on_gate( inputs, output, type );
      return true;
    }

    /* (<string> = <string> */
    if ( std::regex_search( line, m, bench_regex::gate_asgn ) )
    {
      const auto output = detail::trim_copy( m[1] );
      const auto input = detail::trim_copy( m[2] );
      reader.on_assign( input, output );
      return true;
    }

    if ( diag )
    {
      diag->report( diagnostic_level::error,
                    fmt::format( "cannot parse line `{0}`", line ) );
    }

    result = return_code::parse_error;
    return true;
  } );

  return result;
}

inline return_code read_bench( const std::string& filename, const bench_reader& reader, diagnostic_engine* diag = nullptr )
{
  std::ifstream in( filename.c_str(), std::ifstream::in );
  return read_bench( in, reader, diag );
}

} // namespace lorina
