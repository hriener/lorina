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
  \file aig.hpp
  \brief Implements aig parser

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

class aiger_reader
{
public:
  enum latch_init_value
  {
    ZERO = 0
  , ONE
  , NONDETERMINISTIC
  };

public:
  virtual void on_header( std::size_t m, std::size_t i, std::size_t l, std::size_t o, std::size_t a ) const
  {
    (void)m;
    (void)i;
    (void)l;
    (void)o;
    (void)a;
  }

  virtual void on_header( std::size_t m, std::size_t i, std::size_t l, std::size_t o, std::size_t a,
                          std::size_t b, std::size_t c, std::size_t j, std::size_t f ) const
  {
    on_header( m, i, l, o, a );
    (void)b;
    (void)c;
    (void)j;
    (void)f;
  }

  virtual void on_output( unsigned index, unsigned lit ) const
  {
    (void)index;
    (void)lit;
  }

  virtual void on_latch( unsigned index, unsigned next, latch_init_value init_value ) const
  {
    (void)index;
    (void)next;
    (void)init_value;
  }

  virtual void on_and( unsigned index, unsigned left_lit, unsigned right_lit ) const
  {
    (void)index;
    (void)left_lit;
    (void)right_lit;
  }

  virtual void on_input_name( unsigned index, const std::string& name ) const
  {
    (void)index;
    (void)name;
  }

  virtual void on_latch_name( unsigned index, const std::string& name ) const
  {
    (void)index;
    (void)name;
  }

  virtual void on_output_name( unsigned index, const std::string& name ) const
  {
    (void)index;
    (void)name;
  }

  virtual void on_bad_state_name( unsigned index, const std::string& name ) const
  {
    (void)index;
    (void)name;
  }

  virtual void on_constraint_name( unsigned index, const std::string& name ) const
  {
    (void)index;
    (void)name;
  }

  virtual void on_fairness_name( unsigned index, const std::string& name ) const
  {
    (void)index;
    (void)name;
  }

  virtual void on_comment( const std::string& comment ) const
  {
    (void)comment;
  }
}; /* aiger_reader */

class ascii_aiger_pretty_printer : public aiger_reader
{
public:
  ascii_aiger_pretty_printer( std::ostream& os = std::cout )
      : _os( os )
  {
  }

  void on_header( std::size_t m, std::size_t i, std::size_t l, std::size_t o, std::size_t a,
                          std::size_t b, std::size_t c, std::size_t j, std::size_t f ) const override
  {
    _os << fmt::format( "aag {0} {1} {2} {3} {4} {5} {6} {7} {8} ",
                        m, i, l, o, a, b, c, j, f )
        << std::endl;
    for ( auto index = 1u; index <= i; ++index )
    {
      _os << ( 2u * index ) << std::endl;
    }
  }

  void on_output( unsigned index, unsigned lit ) const override
  {
    (void)index;
    _os << lit << std::endl;
  }

  void on_latch( unsigned index, unsigned next, latch_init_value init_value ) const override
  {
    _os << ( 2u * index ) << ' ' << next;
    switch( init_value )
    {
    case 0:
      _os << '0';
      break;
    case 1:
      _os << '1';
      break;
    default:
      break;
    }
    _os << std::endl;
  }

  void on_and( unsigned index, unsigned left_lit, unsigned right_lit ) const override
  {
    _os << ( 2u * index ) << ' ' << left_lit << ' ' << right_lit << std::endl;
  }

  void on_input_name( unsigned index, const std::string& name ) const override
  {
    _os << "i" << index << ' ' << name << std::endl;
  }

  void on_latch_name( unsigned index, const std::string& name ) const override
  {
    _os << "l" << index << ' ' << name << std::endl;
  }

  void on_output_name( unsigned index, const std::string& name ) const override
  {
    _os << "o" << index << ' ' << name << std::endl;
  }

  void on_bad_state_name( unsigned index, const std::string& name ) const override
  {
    _os << "b" << index << ' ' << name << std::endl;
  }

  void on_constraint_name( unsigned index, const std::string& name ) const override
  {
    _os << "c" << index << ' ' << name << std::endl;
  }

  void on_fairness_name( unsigned index, const std::string& name ) const override
  {
    _os << "f" << index << ' ' << name << std::endl;
  }

  void on_comment( const std::string& comment ) const override
  {
    _os << "c" << std::endl
        << comment << std::endl;
  }

  std::ostream& _os;
}; /* ascii_aiger_pretty_printer */

namespace aig_regex
{
static std::regex header( R"(^aig (\d+) (\d+) (\d+) (\d+) (\d+)( \d+)?( \d+)?( \d+)?( \d+)?$)" );
static std::regex input( R"(^i(\d+) (.*)$)" );
static std::regex latch( R"(^l(\d+) (.*)$)" );
static std::regex output( R"(^o(\d+) (.*)$)" );
static std::regex bad_state( R"(^b(\d+) (.*)$)" );
static std::regex constraint( R"(^c(\d+) (.*)$)" );
static std::regex fairness( R"(^f(\d+) (.*)$)" );
} // namespace aig_regex

inline return_code read_aig( std::istream& in, const aiger_reader& reader, diagnostic_engine* diag = nullptr )
{
  return_code result = return_code::success;

  std::smatch m;
  std::string header_line;
  std::getline( in, header_line );

  std::size_t _m, _i, _l, _o, _a, _b, _c, _j, _f;

  /*** parse header ***/
  if ( std::regex_search( header_line, m, aig_regex::header ) )
  {
    std::vector<std::size_t> header;
    for ( const auto& i : m )
    {
      if ( i == "" )
        continue;
      header.push_back( std::atol( std::string( i ).c_str() ) );
    }

    assert( header.size() >= 6u );
    assert( header.size() <= 10u );

    _m = header[1u];
    _i = header[2u];
    _l = header[3u];
    _o = header[4u];
    _a = header[5u];
    _b = header.size() > 6 ? header[6u] : 0ul;
    _c = header.size() > 7 ? header[7u] : 0ul;
    _j = header.size() > 8 ? header[8u] : 0ul;
    _f = header.size() > 9 ? header[9u] : 0ul;
    reader.on_header( _m, _i, _l, _o, _a, _b, _c, _j, _f );
  }
  else
  {
    if ( diag )
    {
      diag->report( diagnostic_level::fatal,
                    fmt::format( "could not parse AIGER header `{0}`", header_line ) );
    }
    return return_code::parse_error;
  }

  std::string line;

  /* latches */
  for ( auto i = 0ul; i < _l; ++i )
  {
    std::getline( in, line );
    const auto tokens = detail::split( line, " " );
    const auto next = std::atoi( tokens[0u].c_str() );
    aiger_reader::latch_init_value init_value = aiger_reader::latch_init_value::NONDETERMINISTIC;
    if ( tokens.size() == 2u )
    {
      if ( tokens[1u] == "0" )
      {
        init_value = aiger_reader::latch_init_value::ZERO;
      }
      else if ( tokens[1u] == "1" )
      {
        init_value = aiger_reader::latch_init_value::ONE;
      }
    }

    reader.on_latch( _i + i + 1u, next, init_value );
  }

  for ( auto i = 0ul; i < _o; ++i )
  {
    std::getline( in, line );
    reader.on_output( i, std::atol( line.c_str() ) );
  }

  /* ignore b */
  for ( auto i = 0ul; i < _b; ++i )
  {
    std::getline( in, line );
  }

  /* ignore c */
  for ( auto i = 0ul; i < _c; ++i )
  {
    std::getline( in, line );
  }

  /* ignore j */
  for ( auto i = 0ul; i < _j; ++i )
  {
    std::getline( in, line );
  }

  /* ignore f */
  for ( auto i = 0ul; i < _f; ++i )
  {
    std::getline( in, line );
  }

  const auto decode = [&]() {
    auto i = 0;
    auto res = 0;
    while ( true )
    {
      auto c = in.get();

      res |= ( ( c & 0x7f ) << ( 7 * i ) );
      if ( ( c & 0x80 ) == 0 )
        break;

      ++i;
    }
    return res;
  };

  /* and gates */
  for ( auto i = _i + _l + 1; i < _i + _l + _a + 1; ++i )
  {
    const auto d1 = decode();
    const auto d2 = decode();
    const auto g = i << 1;
    reader.on_and( i, ( g - d1 ), ( g - d1 - d2 ) );
  }

  /* parse names and comments */
  while ( std::getline( in, line ) )
  {
    if ( std::regex_search( line, m, aig_regex::input ) )
    {
      reader.on_input_name( std::atol( std::string( m[1u] ).c_str() ), m[2u] );
    }
    else if ( std::regex_search( line, m, aig_regex::latch ) )
    {
      reader.on_latch_name( std::atol( std::string( m[1u] ).c_str() ), m[2u] );
    }
    else if ( std::regex_search( line, m, aig_regex::output ) )
    {
      reader.on_output_name( std::atol( std::string( m[1u] ).c_str() ), m[2u] );
    }
    else if ( std::regex_search( line, m, aig_regex::bad_state ) )
    {
      reader.on_bad_state_name( std::atol( std::string( m[1u] ).c_str() ), m[2u] );
    }
    else if ( std::regex_search( line, m, aig_regex::constraint ) )
    {
      reader.on_constraint_name( std::atol( std::string( m[1u] ).c_str() ), m[2u] );
    }
    else if ( std::regex_search( line, m, aig_regex::fairness ) )
    {
      reader.on_fairness_name( std::atol( std::string( m[1u] ).c_str() ), m[2u] );
    }
    else if ( line == "c" )
    {
      std::string comment = "";
      while ( std::getline( in, line ) )
      {
        comment += line;
      }
      reader.on_comment( comment );
      break;
    }
  }

  return result;
}

inline return_code read_aig( const std::string& filename, const aiger_reader& reader, diagnostic_engine* diag = nullptr )
{
  std::ifstream in( filename.c_str(), std::ifstream::in );
  return read_aig( in, reader, diag );
}

} // namespace lorina
