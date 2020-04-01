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
  \file bristol.hpp
  \brief Implements a parser for Bristol fashion

  \author Heinz Riener

  Reference: https://homes.esat.kuleuven.be/~nsmart/MPC/
*/

#pragma once

#include "common.hpp"

#include <fstream>
#include <iostream>

namespace lorina
{

class bristol_reader
{
public:
  explicit bristol_reader() {}

  virtual ~bristol_reader() {}

  virtual void on_header( uint32_t num_gates, uint32_t num_wires, uint32_t num_inputs, uint32_t num_wires_per_input, uint32_t num_outputs, uint32_t num_wires_per_output ) const
  {
    (void)num_gates;
    (void)num_wires;
    (void)num_inputs;
    (void)num_wires_per_input;
    (void)num_outputs;
    (void)num_wires_per_output;
  }

  virtual void on_gate( std::vector<uint32_t> const& in, uint32_t out, std::string const& gate ) const
  {
    (void)in;
    (void)out;
    (void)gate;
  }
};

class bristol_parser
{
public:
  explicit bristol_parser( std::istream& is, bristol_reader const& reader )
    : is( is )
    , reader( reader )
  {
  }

  bool get_tokens_of_next_line( std::vector<std::string>& tokens )
  {
    tokens.clear();

    std::string line;
    do
    {
      if ( is.eof() )
        return false;

      std::getline( is, line );
    } while ( line == "" );

    std::istringstream iss( line );
    std::string token;
    while ( !iss.eof() )
    {
      iss >> token;
      tokens.push_back( token );
    }

    return true;
  }

  return_code run()
  {
    std::vector<std::string> tokens;

    /* read header */
    if ( !get_tokens_of_next_line( tokens ) || tokens.size() != 2u )
      return return_code::parse_error;

    uint32_t num_gates = std::atoi( tokens[0u].c_str() );
    uint32_t num_wires = std::atoi( tokens[1u].c_str() );

    if ( !get_tokens_of_next_line( tokens ) || tokens.size() != 2u )
      return return_code::parse_error;

    uint32_t num_inputs = std::atoi( tokens[0u].c_str() );
    uint32_t num_inputs_per_wire = std::atoi( tokens[1u].c_str() );

    if ( !get_tokens_of_next_line( tokens ) || tokens.size() != 2u )
      return return_code::parse_error;

    uint32_t num_outputs = std::atoi( tokens[0u].c_str() );
    uint32_t num_outputs_per_wire = std::atoi( tokens[1u].c_str() );

    reader.on_header( num_gates, num_wires, num_inputs, num_inputs_per_wire, num_outputs, num_outputs_per_wire );

    /* parse gates */
    while ( get_tokens_of_next_line( tokens ) )
    {
      uint32_t num_input_wires = std::atoi( tokens[0u].c_str() );
      uint32_t num_output_wires = std::atoi( tokens[1u].c_str() );

      assert( tokens.size() == num_input_wires + num_output_wires + 3u );
      assert( num_output_wires == 1u );

      std::vector<uint32_t> inputs;
      for ( uint32_t i = 0; i < num_input_wires; ++i )
      {
        inputs.push_back( std::atoi( tokens[ 2u + i ].c_str() ) );
      }

      uint32_t const output = std::atoi( tokens[ 2u + num_input_wires ].c_str() );
      std::string const& gate = tokens[tokens.size() - 1];

      reader.on_gate( inputs, output, gate );
    }

    return return_code::success;
  }

private:
  std::istream& is;
  bristol_reader const& reader;
};

/*! \brief Reads a Bristol fashion circuit from a stream
 *
 * A callback method of the reader visitor is invoked for each
 * primitive object matched in the specification.
 */
inline return_code read_bristol( std::istream& is, bristol_reader const& reader )
{
  return bristol_parser( is, reader ).run();
}

/*! \brief Reads a Bristol fashion circuit from a file
 *
 * A callback method of the reader visitor is invoked for each
 * primitive object matched in the specification.
 */
inline return_code read_bristol( std::string const& filename, bristol_reader const& reader )
{
  std::ifstream is( filename, std::ifstream::in );
  return read_bristol( is, reader );
}

} // namespace lorina
