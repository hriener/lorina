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

namespace lorina
{

class bristol_reader
{
public:
  explicit bristol_reader() {}

protected:
};

/*! \brief Reads a Bristol fashion circuit from a stream
 *
 * A callback method of the reader visitor is invoked for each
 * primitive object matched in the specification.
 */
inline return_code read_bristol( std::istream& is, bristol_reader const& reader )
{
  assert( false );
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
