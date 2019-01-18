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
  \file liberty.hpp
  \brief Implements liberty parser

  \author Heinz Riener
*/

#pragma once

#include <lorina/common.hpp>
#include <lorina/diagnostics.hpp>
#include <lorina/detail/utils.hpp>
#include <fstream>
#include <iostream>
#include <regex>

namespace lorina
{

/*! \brief A reader visitor for the LIBERTY format.
 *
 * Callbacks for the LIBERTY format.
 */
class liberty_reader
{
public:
}; // liberty_reader

/*! \brief A LIBERTY reader for pretty-printing.
 *
 * Callbacks for pretty-printing LIBERTY.
 */
class liberty_pretty_printer : public liberty_reader
{
public:
}; // liberty_pretty_printer

/*! \brief Reader function for LIBERTY format.
 *
 * Reads LIBERTY format from a stream and invokes a callback
 * method for each parsed primitive and each detected parse error.
 *
 * \param in Input stream
 * \param reader A LIBERTY reader with callback methods invoked for parsed primitives
 * \param diag An optional diagnostic engine with callback methods for parse errors
 * \return Success if parsing have been successful, or parse error if parsing have failed
 */
inline return_code read_liberty( std::istream& in, const liberty_reader& reader, diagnostic_engine* diag = nullptr )
{
  (void)in;
  (void)reader;
  (void)diag;
  return return_code::parse_error;
}

/*! \brief Reader function for LIBERTY format.
 *
 * Reads LIBERTY format from a file and invokes a callback method for each
 * parsed primitive and each detected parse error.
 *
 * \param filename Name of the file
 * \param reader A LIBERTY reader with callback methods invoked for parsed primitives
 * \param diag An optional diagnostic engine with callback methods for parse errors
 * \return Success if parsing have been successful, or parse error if parsing have failed
 */
inline return_code read_liberty( const std::string& filename, const liberty_reader& reader, diagnostic_engine* diag = nullptr )
{
  (void)diag;

  std::ifstream in( detail::word_exp_filename( filename ), std::ifstream::in );
  return read_liberty( in, reader );
}

} // namespace lorina
