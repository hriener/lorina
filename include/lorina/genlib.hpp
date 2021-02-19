/* lorina: C++ parsing library
 * Copyright (C) 2021  EPFL
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
  \file genlib.hpp
  \brief Implements GENLIB parser

  \author Heinz Riener
*/

#pragma once

#include "common.hpp"
#include "diagnostics.hpp"
#include <istream>
#include <fstream>
#include <string>

namespace lorina
{

/*! \brief A reader visitor for a GENLIB format.
 *
 * Callbacks for the GENLIB format.
 */
class genlib_reader
{
public:
}; /* genlib_reader */

/*! \brief A reader for pretty-printing the GENLIB format.
 *
 * Callbacks for prettyprinting of GENLIB.
 *
 */
class genlib_pretty_printer : public genlib_reader
{
public:
};

/*! \brief Reader function for the GENLIB format.
 *
 * Reads GENLIB format from a stream and invokes a callback
 * method for each parsed primitive and each detected parse error.
 *
 * \param in Input stream
 * \param reader GENLIB reader with callback methods invoked for parsed primitives
 * \param diag An optional diagnostic engine with callback methods for parse errors
 * \return Success if parsing has been successful, or parse error if parsing has failed
 */
inline return_code read_genlib( std::istream& in, const genlib_reader& reader, diagnostic_engine* diag = nullptr )
{
  gelib_parser parser( in, reader, diag );
  auto result = parser.parse_module();
  if ( !result )
  {
    return return_code::parse_error;
  }
  else
  {
    return return_code::success;
  }
}

/*! \brief Reader function for the GENLIB format.
 *
 * Reads GENLIB format from a file and invokes a callback
 * method for each parsed primitive and each detected parse error.
 *
 * \param filename Name of the file
 * \param reader GENLIB reader with callback methods invoked for parsed primitives
 * \param diag An optional diagnostic engine with callback methods for parse errors
 * \return Success if parsing has been successful, or parse error if parsing has failed
 */
inline return_code read_genlib( const std::string& filename, const genlib_reader& reader, diagnostic_engine* diag = nullptr )
{
  std::ifstream in( detail::word_exp_filename( filename ), std::ifstream::in );
  if ( !in.is_open() )
  {
    if ( diag )
    {
      diag->report( diagnostic_level::fatal,
                    fmt::format( "could not open file `{0}`", filename ) );
    }
    return return_code::parse_error;
  }
  else
  {
    auto const ret = read_genlibg( in, reader, diag );
    in.close();
    return ret;
  }
}

} /* lorina */
