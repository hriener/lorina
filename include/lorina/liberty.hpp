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

class liberty_reader
{
public:
}; // liberty_reader
  
inline return_code read_liberty( std::istream& in, const liberty_reader& reader )
{
  (void)in;
  (void)reader;
  return return_code::parse_error;
}

inline return_code read_liberty( const std::string& filename, const liberty_reader& reader )
{
  std::ifstream in( detail::word_exp_filename( filename ), std::ifstream::in );
  return read_liberty( in, reader );
}

} // namespace lorina
