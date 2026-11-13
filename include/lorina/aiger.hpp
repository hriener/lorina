/* lorina: C++ parsing library
 * Copyright (C) 2018-2021  EPFL
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
  \file aiger.hpp
  \brief Implements Aiger parser

  \author Heinz Riener
  \author Mathias Soeken
  \author Siang-Yun (Sonia) Lee
*/

#pragma once

#include "common.hpp"
#include "diagnostics.hpp"
#include "detail/utils.hpp"
#include <array>
#include <charconv>
#include <cstdint>
#include <limits>
#include <string_view>
#include <system_error>
#include <fstream>
#include <iostream>
#include <regex>

namespace lorina
{

/*! \brief A reader visitor for the binary AIGER format.
 *
 * Callbacks for the AIGER format.
 */
class aiger_reader
{
public:
  /*! Latch input values */
  enum latch_init_value
  {
    ZERO = 0 /*!< Initialized with 0 */
  , ONE /*!< Initialized with 1 */
  , NONDETERMINISTIC /*!< Not initialized (non-deterministic value) */
  };

public:
  /*! \brief Callback method for parsed header.
   *
   * \param m Maximum variable index
   * \param i Number of inputs
   * \param l Number of latches
   * \param o Number of outputs
   * \param a Number of AND gates
   */
  virtual void on_header( uint64_t m, uint64_t i, uint64_t l, uint64_t o, uint64_t a ) const
  {
    (void)m;
    (void)i;
    (void)l;
    (void)o;
    (void)a;
  }

  /*! \brief Callback method for parsed header.
   *
   * \param m Maximum variable index
   * \param i Number of inputs
   * \param l Number of latches
   * \param o Number of outputs
   * \param a Number of AND gates
   * \param b Number of bad states properties
   * \param c Number of invariant constraints
   * \param j Number of justice properties
   * \param f Number of fairness constraints
   */
  virtual void on_header( uint64_t m, uint64_t i, uint64_t l, uint64_t o, uint64_t a,
                          uint64_t b, uint64_t c, uint64_t j, uint64_t f ) const
  {
    on_header( m, i, l, o, a );
    (void)b;
    (void)c;
    (void)j;
    (void)f;
  }

  /*! \brief Callback method for parsed input.
   *
   * \param pos Position in input list
   * \param lit Assigned literal
   */
  virtual void on_input( uint32_t pos, uint32_t lit ) const
  {
    (void)pos;
    (void)lit;
  }

  /*! \brief Callback method for parsed output.
   *
   * \param pos Position in output list
   * \param lit Assigned literal
   */
  virtual void on_output( uint32_t pos, uint32_t lit ) const
  {
    (void)pos;
    (void)lit;
  }

  /*! \brief Callback method for parsed latch.
   *
   * \param index Index of the latch
   * \param next Assigned (next) literal
   * \param reset Initial value of the latch
   */
  virtual void on_latch( uint32_t index, uint32_t next, latch_init_value reset ) const
  {
    (void)index;
    (void)next;
    (void)reset;
  }

  /*! \brief Callback method for parsed AND gate.
   *
   * \param index Index of the AND gate
   * \param left_lit Assigned left literal
   * \param right_lit Assigned right literal
   */
  virtual void on_and( uint32_t index, uint32_t left_lit, uint32_t right_lit ) const
  {
    (void)index;
    (void)left_lit;
    (void)right_lit;
  }

  /*! \brief Callback method for parsed bad state property.
   *
   * \param index Position in the list of bad state property
   * \param lit Assigned literal
   */
  virtual void on_bad_state( uint32_t pos, uint32_t lit ) const
  {
    (void)pos;
    (void)lit;
  }

  /*! \brief Callback method for parsed constraint.
   *
   * \param index Position in the list of constraint
   * \param lit Assigned literal
   */
  virtual void on_constraint( uint32_t pos, uint32_t lit ) const
  {
    (void)pos;
    (void)lit;
  }

  /*! \brief Callback method for parsed fairness constraints.
   *
   * \param index Position in the list of fairness constraint
   * \param lit Assigned literal
   */
  virtual void on_fairness( uint32_t pos, uint32_t lit ) const
  {
    (void)pos;
    (void)lit;
  }

  /*! \brief Callback method for parsed header of justice property.
   *
   * \param index Position in the list of the justice property
   * \param size Number of assigned literals
   */
  virtual void on_justice_header( uint32_t pos, uint64_t size ) const
  {
    (void)pos;
    (void)size;
  }

  /*! \brief Callback method for parsed justice property.
   *
   * \param index Position in the list of the justice property
   * \param lits Assigned literals
   */
  virtual void on_justice( uint32_t pos, const std::vector<uint32_t>& lits ) const
  {
    (void)pos;
    (void)lits;
  }

  /*! \brief Callback method for parsed input name.
   *
   * \param pos Position in input list
   * \param name Input name
   */
  virtual void on_input_name( uint32_t pos, const std::string& name ) const
  {
    (void)pos;
    (void)name;
  }

  /*! \brief Callback method for parsed latch name.
   *
   * \param pos Position in latch list
   * \param name Latch name
   */
  virtual void on_latch_name( uint32_t pos, const std::string& name ) const
  {
    (void)pos;
    (void)name;
  }

  /*! \brief Callback method for parsed output name.
   *
   * \param pos Position in output list
   * \param name Output name
   */
  virtual void on_output_name( uint32_t pos, const std::string& name ) const
  {
    (void)pos;
    (void)name;
  }

  /*! \brief Callback method for a parsed name of a bad state property.
   *
   * \param pos Position in list of bad state property
   * \param name Name of the bad state property
   */
  virtual void on_bad_state_name( uint32_t pos, const std::string& name ) const
  {
    (void)pos;
    (void)name;
  }

  /*! \brief Callback method for a parsed name of an invariant constraint.
   *
   * \param pos Position in constraint list
   * \param name Constraint name
   */
  virtual void on_constraint_name( uint32_t pos, const std::string& name ) const
  {
    (void)pos;
    (void)name;
  }

  /*! \brief Callback method for a parsed name of a justice property.
   *
   * \param pos Position in the list of justice property
   * \param name Name of the fairness constraint
   */
  virtual void on_justice_name( uint32_t pos, const std::string& name ) const
  {
    (void)pos;
    (void)name;
  }

  /*! \brief Callback method for a parsed name of a fairness constraint.
   *
   * \param pos Position in list of fairness constraint
   * \param name Name of the fairness constraint
   */
  virtual void on_fairness_name( uint32_t pos, const std::string& name ) const
  {
    (void)pos;
    (void)name;
  }

  /*! \brief Callback method for parsed comment.
   *
   * \param comment Comment
   */
  virtual void on_comment( const std::string& comment ) const
  {
    (void)comment;
  }
}; /* aiger_reader */

/*! \brief An AIGER reader for prettyprinting ASCII AIGER.
 *
 * Callbacks for prettyprinting of ASCII AIGER.
 *
 */
class ascii_aiger_pretty_printer : public aiger_reader
{
public:
  /*! \brief Constructor of the ASCII AIGER pretty printer.
   *
   * \param os Output stream
   */
  ascii_aiger_pretty_printer( std::ostream& os = std::cout )
      : _os( os )
  {
  }

  void on_header( uint64_t m, uint64_t i, uint64_t l, uint64_t o, uint64_t a,
                  uint64_t b, uint64_t c, uint64_t j, uint64_t f ) const override
  {
    _os << fmt::format( "aag {0} {1} {2} {3} {4} {5} {6} {7} {8}",
                        m, i, l, o, a, b, c, j, f )
        << std::endl;
  }

  void on_input( uint32_t index, uint32_t lit ) const override
  {
    (void)index;
    _os << lit << std::endl;
  }

  void on_output( uint32_t index, uint32_t lit ) const override
  {
    (void)index;
    _os << lit << std::endl;
  }

  void on_latch( uint32_t index, uint32_t next, latch_init_value init_value ) const override
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

  void on_and( uint32_t index, uint32_t left_lit, uint32_t right_lit ) const override
  {
    _os << ( 2u * index ) << ' ' << left_lit << ' ' << right_lit << std::endl;
  }

  void on_bad_state( uint32_t index, uint32_t lit ) const override
  {
    (void)index;
    _os << lit << std::endl;
  }

  void on_constraint( uint32_t index, uint32_t lit ) const override
  {
    (void)index;
    _os << lit << std::endl;
  }

  void on_justice_header( uint32_t index, uint64_t size ) const override
  {
    (void)index;
    _os << size << std::endl;
  }

  void on_justice( uint32_t index, const std::vector<uint32_t>& lits ) const override
  {
    (void)index;
    for ( const auto& l : lits )
    {
      _os << l << std::endl;
    }
  }

  void on_fairness( uint32_t index, uint32_t lit ) const override
  {
    (void)index;
    _os << lit << std::endl;
  }

  void on_input_name( uint32_t pos, const std::string& name ) const override
  {
    _os << "i" << pos << ' ' << name << std::endl;
  }

  void on_latch_name( uint32_t pos, const std::string& name ) const override
  {
    _os << "l" << pos << ' ' << name << std::endl;
  }

  void on_output_name( uint32_t pos, const std::string& name ) const override
  {
    _os << "o" << pos << ' ' << name << std::endl;
  }

  void on_bad_state_name( uint32_t pos, const std::string& name ) const override
  {
    _os << "b" << pos << ' ' << name << std::endl;
  }

  void on_constraint_name( uint32_t pos, const std::string& name ) const override
  {
    _os << "c" << pos << ' ' << name << std::endl;
  }

  void on_justice_name( uint32_t pos, const std::string& name ) const override
  {
    _os << "j" << pos << ' ' << name << std::endl;
  }

  void on_fairness_name( uint32_t pos, const std::string& name ) const override
  {
    _os << "f" << pos << ' ' << name << std::endl;
  }

  void on_comment( const std::string& comment ) const override
  {
    _os << "c" << std::endl
        << comment << std::endl;
  }

  std::ostream& _os; /*!< Output stream */
}; /* ascii_aiger_pretty_printer */

namespace aig_regex
{
static std::regex header( R"(^aig (\d+) (\d+) (\d+) (\d+) (\d+)( \d+)?( \d+)?( \d+)?( \d+)?$)" );
static std::regex ascii_header( R"(^aag (\d+) (\d+) (\d+) (\d+) (\d+)( \d+)?( \d+)?( \d+)?( \d+)?$)" );
static std::regex input( R"(^i(\d+) (.*)$)" );
static std::regex latch( R"(^l(\d+) (.*)$)" );
static std::regex output( R"(^o(\d+) (.*)$)" );
static std::regex bad_state( R"(^b(\d+) (.*)$)" );
static std::regex constraint( R"(^c(\d+) (.*)$)" );
static std::regex justice( R"(^j(\d+) (.*)$)" );
static std::regex fairness( R"(^f(\d+) (.*)$)" );
} // namespace aig_regex

/*! \brief Reader function for ASCII AIGER format.
 *
 * Reads ASCII AIGER format from a stream and invokes a callback
 * method for each parsed primitive and each detected parse error.
 *
 * \param in Input stream
 * \param reader An AIGER reader with callback methods invoked for parsed primitives
 * \param diag An optional diagnostic engine with callback methods for parse errors
 * \return Success if parsing has been successful, or parse error if parsing has failed
 */
[[nodiscard]] inline return_code read_ascii_aiger( std::istream& in, const aiger_reader& reader, diagnostic_engine* diag = nullptr )
{
  return_code result = return_code::success;

  std::smatch m;
  std::string header_line;
  detail::getline( in, header_line );

  uint64_t _m, _i, _l, _o, _a, _b, _c, _j, _f;

  /* header */
  if ( std::regex_search( header_line, m, aig_regex::ascii_header ) )
  {
    std::vector<uint64_t> header;
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
      diag->report( diag_id::ERR_AIGER_HEADER ).add_argument( header_line );
    }
    return return_code::parse_error;
  }

  std::string line;

  /* inputs */
  for ( auto i = 0ul; i < _i; ++i )
  {
    detail::getline( in, line );
    const auto index = std::atol( line.c_str() );
    reader.on_input( i, index );
  }

  /* latches */
  for ( auto i = 0ul; i < _l; ++i )
  {
    detail::getline( in, line );
    const auto tokens = detail::split( line,  " " );

    if ( !(tokens.size() <= 3u) )
    {
      if ( diag )
      {
        diag->report( diag_id::ERR_AIGER_LATCH_DECLARATION ).add_argument( line );
      }
      return return_code::parse_error;
    }

    const auto index = std::atol( std::string(tokens[0u]).c_str() ) / 2u;
    const auto next_lit = std::atol( std::string(tokens[1u]).c_str() );

    aiger_reader::latch_init_value init_value = aiger_reader::latch_init_value::NONDETERMINISTIC;
    if ( tokens.size() == 3u )
    {
      if ( tokens[2u] == "0" )
      {
        init_value = aiger_reader::latch_init_value::ZERO;
      }
      else if ( tokens[1u] == "1" )
      {
        init_value = aiger_reader::latch_init_value::ONE;
      }
    }

    reader.on_latch( index, next_lit, init_value );
  }

  /* outputs */
  for ( auto i = 0ul; i < _o; ++i )
  {
    detail::getline( in, line );
    const auto lit = std::atol( line.c_str() );
    reader.on_output( i, lit );
  }

  /* bad state properties */
  for ( auto i = 0ul; i < _b; ++i )
  {
    detail::getline( in, line );
    const auto lit = std::atol( line.c_str() );
    reader.on_bad_state( i, lit );
  }

  /* constraints */
  for ( auto i = 0ul; i < _c; ++i )
  {
    detail::getline( in, line );
    const auto lit = std::atol( line.c_str() );
    reader.on_constraint( i, lit );
  }

  /* justice properties */
  std::vector<uint64_t> justice_sizes;
  for ( auto i = 0ul; i < _j; ++i )
  {
    detail::getline( in, line );
    const auto justice_size = std::atol( line.c_str() );
    justice_sizes.emplace_back( justice_size );
    reader.on_justice_header( i, justice_size );
  }

  for ( auto i = 0ul; i < _j; ++i )
  {
    std::vector<uint32_t> lits;
    for ( auto j = 0ul; j < justice_sizes[i]; ++j )
    {
      detail::getline( in, line );
      const auto lit = std::atol( line.c_str() );
      lits.emplace_back( lit );
    }
    reader.on_justice( i, lits );
  }

  /* fairness */
  for ( auto i = 0ul; i < _f; ++i )
  {
    detail::getline( in, line );
    const auto lit = std::atol( line.c_str() );
    reader.on_fairness( i, lit );
  }

  /* ands */
  for ( auto i = 0ul; i < _a; ++i )
  {
    detail::getline( in, line );
    const auto tokens = detail::split( line, " " );

    if ( !(tokens.size() == 3u) )
    {
      if ( diag )
      {
        diag->report( diag_id::ERR_AIGER_AND_DECLARATION ).add_argument( line );
      }
      return return_code::parse_error;
    }

    const auto index = std::atol( std::string(tokens[0u]).c_str() )/2u;
    const auto left_lit = std::atol( std::string(tokens[1u]).c_str() );
    const auto right_lit = std::atol( std::string(tokens[2u]).c_str() );
    reader.on_and( index, left_lit, right_lit );
  }

  /* parse names and comments */
  while ( detail::getline( in, line ) )
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
    else if ( std::regex_search( line, m, aig_regex::justice ) )
    {
      reader.on_justice_name( std::atol( std::string( m[1u] ).c_str() ), m[2u] );
    }
    else if ( std::regex_search( line, m, aig_regex::fairness ) )
    {
      reader.on_fairness_name( std::atol( std::string( m[1u] ).c_str() ), m[2u] );
    }
    else if ( line == "c" )
    {
      std::string comment = "";
      while ( detail::getline( in, line ) )
      {
        comment += line;
      }
      reader.on_comment( comment );
      break;
    }
  }

  return result;
}

/*! \brief Reader function for ASCII AIGER format.
 *
 * Reads ASCII AIGER format from a file and invokes a callback
 * method for each parsed primitive and each detected parse error.
 *
 * \param filename Name of the file
 * \param reader An AIGER reader with callback methods invoked for parsed primitives
 * \param diag An optional diagnostic engine with callback methods for parse errors
 * \return Success if parsing has been successful, or parse error if parsing has failed
 */
[[nodiscard]] inline return_code read_ascii_aiger( const std::string& filename, const aiger_reader& reader, diagnostic_engine* diag = nullptr )
{
  std::ifstream in( detail::word_exp_filename( filename ), std::ifstream::in );
  if ( !in.is_open() )
  {
    if ( diag )
    {
      diag->report( diag_id::ERR_FILE_OPEN ).add_argument( filename );
    }
    return return_code::parse_error;
  }
  else
  {
    auto const ret = read_ascii_aiger( in, reader, diag );
    in.close();
    return ret;
  }
}

/*! \brief Reader function for binary AIGER format.
 *
 * Reads binary AIGER format from a stream and invokes a callback
 * method for each parsed primitive and each detected parse error.
 * Checks header counts, literal and symbol bounds, latch resets, and gate deltas.
 * Callbacks may have already received a prefix of the input on failure.
 * Header counts describe the circuit size and can be large even for short inputs.
 *
 * \param in Input stream
 * \param reader An AIGER reader with callback methods invoked for parsed primitives
 * \param diag An optional diagnostic engine with callback methods for parse errors
 * \return Success if parsing has been successful, or parse error if parsing has failed
 */
[[nodiscard]] inline return_code read_aiger( std::istream& in, const aiger_reader& reader, diagnostic_engine* diag = nullptr )
{
  const auto parse_number = []( std::string_view token, uint32_t& value ) {
    if ( token.empty() )
      return false;
    const auto* end = token.data() + token.size();
    const auto [position, error] = std::from_chars( token.data(), end, value );
    return error == std::errc{} && position == end;
  };

  const auto fail = [&]( diag_id id, std::string const& record ) {
    if ( diag )
      diag->report( id ).add_argument( record );
    return return_code::parse_error;
  };

  std::string line;
  detail::getline( in, line );
  constexpr std::string_view prefix = "aig ";
  if ( line.compare( 0u, prefix.size(), prefix ) != 0 )
    return fail( diag_id::ERR_AIGER_HEADER, line );

  std::array<uint32_t, 9> counts{};
  std::string_view header( line );
  header.remove_prefix( prefix.size() );
  for ( size_t field = 0u;; ++field )
  {
    const auto separator = header.find( ' ' );
    if ( field == counts.size() || !parse_number( header.substr( 0, separator ), counts[field] ) )
      return fail( diag_id::ERR_AIGER_HEADER, line );
    if ( separator == std::string_view::npos )
    {
      if ( field < 4u )
        return fail( diag_id::ERR_AIGER_HEADER, line );
      break;
    }
    header.remove_prefix( separator + 1u );
  }

  const auto [variables, inputs, latches, outputs, gates, bad, constraints, justice, fairness] = counts;
  if ( variables > std::numeric_limits<uint32_t>::max() / 2u ||
       variables != static_cast<uint64_t>( inputs ) + latches + gates )
    return fail( diag_id::ERR_AIGER_HEADER, line );

  reader.on_header( variables, inputs, latches, outputs, gates, bad, constraints, justice, fairness );
  const auto max_literal = 2u * variables + 1u;
  const auto read_literal = [&]( uint32_t& literal ) {
    return detail::getline( in, line ) && parse_number( line, literal ) && literal <= max_literal;
  };

  for ( auto i = 0u; i < inputs; ++i )
    reader.on_input( i, 2u * ( i + 1u ) );

  for ( auto i = 0u; i < latches; ++i )
  {
    detail::getline( in, line );
    const std::string_view record( line );
    const auto separator = record.find( ' ' );
    uint32_t next{}, reset{};
    const auto index = inputs + i + 1u;
    if ( !parse_number( record.substr( 0, separator ), next ) || next > max_literal ||
         ( separator != std::string_view::npos &&
           ( !parse_number( record.substr( separator + 1u ), reset ) || ( reset > 1u && reset != 2u * index ) ) ) )
      return fail( diag_id::ERR_AIGER_LATCH_DECLARATION, line );

    const auto init = reset == 0u ? aiger_reader::ZERO : reset == 1u ? aiger_reader::ONE : aiger_reader::NONDETERMINISTIC;
    reader.on_latch( index, next, init );
  }

  const auto read_literals = [&]( uint32_t count, auto callback ) {
    for ( auto i = 0u; i < count; ++i )
    {
      uint32_t literal{};
      if ( !read_literal( literal ) )
        return false;
      (reader.*callback)( i, literal );
    }
    return true;
  };
  if ( !read_literals( outputs, &aiger_reader::on_output ) ||
       !read_literals( bad, &aiger_reader::on_bad_state ) ||
       !read_literals( constraints, &aiger_reader::on_constraint ) )
    return fail( diag_id::ERR_PARSE_LINE, in.eof() ? "missing AIGER output, bad state, or constraint literal" : line );

  std::vector<uint32_t> justice_sizes;
  for ( auto i = 0u; i < justice; ++i )
  {
    uint32_t size{};
    if ( !detail::getline( in, line ) || !parse_number( line, size ) )
      return fail( diag_id::ERR_PARSE_LINE, in.eof() ? "missing AIGER justice size" : line );
    justice_sizes.push_back( size );
    reader.on_justice_header( i, size );
  }

  for ( auto i = 0u; i < justice; ++i )
  {
    std::vector<uint32_t> literals;
    for ( auto j = 0u; j < justice_sizes[i]; ++j )
    {
      uint32_t literal{};
      if ( !read_literal( literal ) )
        return fail( diag_id::ERR_PARSE_LINE, in.eof() ? "missing AIGER justice literal" : line );
      literals.push_back( literal );
    }
    reader.on_justice( i, literals );
  }
  if ( !read_literals( fairness, &aiger_reader::on_fairness ) )
    return fail( diag_id::ERR_PARSE_LINE, in.eof() ? "missing AIGER fairness literal" : line );

  const auto decode_delta = [&]( uint32_t& value ) {
    value = 0u;
    for ( unsigned shift = 0u; shift < 32u; shift += 7u )
    {
      const auto byte = in.get();
      if ( byte == std::char_traits<char>::eof() || ( shift == 28u && byte > 0x0f ) )
        return false;
      value |= static_cast<uint32_t>( byte & 0x7f ) << shift;
      if ( ( byte & 0x80 ) == 0 )
        return true;
    }
    return false;
  };
  for ( auto i = inputs + latches + 1u; i <= variables; ++i )
  {
    const auto lhs = 2u * i;
    uint32_t first{}, second{};
    if ( !decode_delta( first ) || first == 0u || first > lhs ||
         !decode_delta( second ) || second > lhs - first )
      return fail( diag_id::ERR_AIGER_AND_DECLARATION, std::to_string( lhs ) );
    reader.on_and( i, lhs - first, lhs - first - second );
  }

  constexpr std::string_view symbols = "ilobcjf";
  const std::array<uint32_t, 7> limits{ inputs, latches, outputs, bad, constraints, justice, fairness };
  constexpr std::array callbacks{ &aiger_reader::on_input_name, &aiger_reader::on_latch_name, &aiger_reader::on_output_name,
                                  &aiger_reader::on_bad_state_name, &aiger_reader::on_constraint_name,
                                  &aiger_reader::on_justice_name, &aiger_reader::on_fairness_name };
  while ( !detail::getline( in, line ).eof() && !in.fail() )
  {
    if ( line == "c" )
    {
      std::string comment;
      while ( !detail::getline( in, line ).eof() && !in.fail() )
        comment += line;
      reader.on_comment( comment );
      break;
    }

    if ( line.empty() )
      continue;
    const auto kind = symbols.find( line.front() );
    if ( kind == std::string_view::npos )
      continue;
    const auto separator = line.find( ' ' );
    uint32_t index{};
    if ( separator == std::string::npos ||
         !parse_number( std::string_view( line ).substr( 1u, separator - 1u ), index ) || index >= limits[kind] )
      return fail( diag_id::ERR_PARSE_LINE, line );
    (reader.*callbacks[kind])( index, line.substr( separator + 1u ) );
  }

  return in.bad() || in.fail() ? return_code::parse_error : return_code::success;
}

/*! \brief Reader function for binary AIGER format.
 *
 * Reads binary AIGER format from a file and invokes a callback
 * method for each parsed primitive and each detected parse error.
 *
 * \param filename Name of the file
 * \param reader An AIGER reader with callback methods invoked for parsed primitives
 * \param diag An optional diagnostic engine with callback methods for parse errors
 * \return Success if parsing has been successful, or parse error if parsing has failed
 */
[[nodiscard]] inline return_code read_aiger( const std::string& filename, const aiger_reader& reader, diagnostic_engine* diag = nullptr )
{
  std::ifstream in( detail::word_exp_filename( filename ), std::ifstream::binary );
  if ( !in.is_open() )
  {
    if ( diag )
    {
      diag->report( diag_id::ERR_FILE_OPEN ).add_argument( filename );
    }
    return return_code::parse_error;
  }
  else
  {
    auto const ret = read_aiger( in, reader, diag );
    in.close();
    return ret;
  }
}

} // namespace lorina
