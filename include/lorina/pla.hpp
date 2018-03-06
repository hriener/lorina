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
  \file pla.hpp
  \brief Implements pla parser

  \author Heinz Riener
*/

#pragma once

#include <lorina/common.hpp>
#include <lorina/diagnostics.hpp>
#include <lorina/detail/utils.hpp>
#include <cassert>
#include <fstream>
#include <iostream>
#include <regex>

namespace lorina
{

class pla_reader
{
public:
  virtual void on_number_of_inputs( std::size_t number_of_inputs ) const
  {
    (void)number_of_inputs;
  }

  virtual void on_number_of_outputs( std::size_t number_of_outputs ) const
  {
    (void)number_of_outputs;
  }

  virtual void on_number_of_terms( std::size_t number_of_terms ) const
  {
    (void)number_of_terms;
  }

  virtual void on_end() const {}

  virtual void on_term( const std::string& term, const std::string& out ) const
  {
    (void)term;
    (void)out;
  }
}; /* pla_reader */

namespace pla_regex
{
static std::regex keyword( R"(^\.([^\s]*)(?:\s+(.+))?$)" );
static std::regex term( R"(^([01\-]+)\s+([01-]+)$])" );

} // namespace pla_regex

inline return_code read_pla( std::istream& in, const pla_reader& reader, diagnostic_engine* diag = nullptr )
{
  auto loc = 0ul;
  auto errors = 0ul;

  std::smatch m;
  detail::foreach_line_in_file_escape( in, [&]( const std::string& line ) {
    ++loc;

    /* empty line or comment */
    if ( line.empty() || line[0u] == '#' )
      return true;

    if ( std::regex_search( line, m, pla_regex::keyword ) )
    {
      if ( m[1] == "i" )
      {
        reader.on_number_of_inputs( std::atol( std::string( m[2] ).c_str() ) );
        return true;
      }
      else if ( m[1] == "o" )
      {
        reader.on_number_of_outputs( std::atol( std::string( m[2] ).c_str() ) );
        return true;
      }
      else if ( m[1] == "p" )
      {
        reader.on_number_of_terms( std::atol( std::string( m[2] ).c_str() ) );
        return true;
      }
      else if ( m[1] == "e" )
      {
        reader.on_end();
        return true;
      }
      else
      {
        if ( diag )
          diag->report( diagnostic_level::error,
                        fmt::format( "Unsupported keyword `{2}`\n"
                                     "in line {0}: `{1}`",
                                     loc, line, std::string( m[1] ) ) );
        ++errors;
        return true; /* understood error case, go on parsing */
      }
    }

    /* [01-]+ [01] */
    if ( std::regex_match( line, m, pla_regex::term ) )
    {
      std::string out = m[2];
      reader.on_term( m[1], m[2] );
      return true;
    }

    if ( diag )
      diag->report( diagnostic_level::error,
                    fmt::format( "Unable to parse line\n"
                                 "line {0}: `{1}`",
                                 loc, line ) );
    ++errors;
    return true; /* understood error case, go on parsing */
  } );

  if ( errors > 0 )
  {
    return return_code::parse_error;
  }
  else
  {
    return return_code::success;
  }
}

inline return_code read_pla( const std::string& filename, const pla_reader& reader, diagnostic_engine* diag = nullptr )
{
  std::ifstream in( filename.c_str(), std::ifstream::in );
  return read_pla( in, reader, diag );
}

} // namespace lorina
