/* lorina: C++ parsing library
 * Copyright (C) 2018-2021  EPFL
 * Copyright (C) 2022  Heinz Riener
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
  \file design.hpp
  \brief Design

  \author Heinz Riener
*/

#pragma once

#include <vector>
#include <unordered_map>

namespace lorina
{

/**
 * \brief Module declaration
 *
 * A module declaration consists of a module name, a list of input
 * names, and a list of output names.
 *
 */
class module_decl
{
public:
  explicit module_decl( const std::string& name,
                        const std::vector<std::string>& inputs,
                        const std::vector<std::string>& outputs )
    : name_( name )
    , inputs_( inputs )
    , outputs_( outputs )
  {}

  bool has_input( const std::string& name ) const
  {
    return std::find( std::begin( inputs_ ), std::end( inputs_ ), name ) != std::end( inputs_ );
  }

  bool has_output( const std::string& name ) const
  {
    return std::find( std::begin( outputs_ ), std::end( outputs_ ), name ) != std::end( outputs_ );
  }

protected:
  std::string name_;
  std::vector<std::string> inputs_;
  std::vector<std::string> outputs_;
}; /* module_decl */

/**
 * \brief Container of multiple components defined in an HDL
 *
 * The hdl_context stores the ASTs of multiple HDL components possibly
 * distributed over multiple source files.
 *
 */
class hdl_context
{
public:
  explicit hdl_context() = default;

  /* \brief Declare a prototype of a module in the context */
  void declare_module( const std::string& name, const std::vector<std::string>& inputs, const std::vector<std::string>& outputs )
  {
    modules_.emplace( name, std::make_shared<module_decl>( name, inputs, outputs ) );
  }

  /* \brief Find a module by name in the context */
  std::shared_ptr<module> find_module( const std::string& name ) const
  {
    const auto it = modules_.find( name );
    return it != std::end( modules_ ) ? it->second : nullptr;
  }

protected:
  std::unordered_map<std::string, std::shared_ptr<module_decl>> modules_;
}; /* hdl_context */

} // namespace lorina
