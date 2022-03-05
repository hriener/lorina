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
  \file reader_visitor.hpp
  \brief Reader for Verliog.

  \author Heinz Riener
*/

#pragma once

#include "ast_graph.hpp"
#include <vector>

namespace lorina
{

namespace detail
{

class verilog_reader_visitor
{
public:
  explicit verilog_reader_visitor() = default;
  virtual ~verilog_reader_visitor() = default;

  virtual void on_parameter( ast_id id, ast_id identifier, ast_id expr ) const
  {
    (void)id;
    (void)identifier;
    (void)expr;
  }

  virtual void on_constant( ast_id id, const std::string& value ) const
  {
    (void)id;
    (void)value;
  }

  virtual void on_identifier( ast_id id, const std::string& identifier ) const
  {
    (void)id;
    (void)identifier;
  }

  virtual void on_arithmetic_identifier( ast_id id, const std::string& identifier ) const
  {
    (void)id;
    (void)identifier;
  }

  virtual void on_not( ast_id id, ast_id expr ) const
  {
    (void)id;
    (void)expr;
  }

  virtual void on_and( ast_id id, ast_id a, ast_id b ) const
  {
    (void)id;
    (void)a;
    (void)b;
  }

  virtual void on_or( ast_id id, ast_id a, ast_id b ) const
  {
    (void)id;
    (void)a;
    (void)b;
  }

  virtual void on_xor( ast_id id, ast_id a, ast_id b ) const
  {
    (void)id;
    (void)a;
    (void)b;
  }

  virtual void on_add( ast_id id, ast_id a, ast_id b ) const
  {
    (void)id;
    (void)a;
    (void)b;
  }

  virtual void on_sub( ast_id id, ast_id a, ast_id b ) const
  {
    (void)id;
    (void)a;
    (void)b;
  }

  virtual void on_system_function( ast_id id, const std::string& fun, const std::vector<ast_id>& args ) const
  {
    (void)id;
    (void)fun;
    (void)args;
  }

  virtual void on_assignment( ast_id id, ast_id lhs, ast_id expr ) const
  {
    (void)id;
    (void)lhs;
    (void)expr;
  }

  /*! \brief Callback method for parsed module.
   *
   * \param module_name Name of the module
   * \param inouts Container for input and output names
   */
  virtual void on_module_header( ast_id id, const std::string& module_name, const std::vector<std::string>& inouts ) const
  {
    (void)id;
    (void)module_name;
    (void)inouts;
  }

  /*! \brief Callback method for parsed inputs.
   *
   * \param inputs Input names
   */
  virtual void on_inputs( ast_id id, const std::vector<ast_id>& inputs ) const
  {
    (void)id;
    (void)inputs;
  }

  /*! \brief Callback method for parsed outputs.
   *
   * \param outputs Output names
   */
  virtual void on_outputs( ast_id id, const std::vector<ast_id>& outputs ) const
  {
    (void)id;
    (void)outputs;
  }

  /*! \brief Callback method for parsed wires.
   *
   * \param wires Wire names
   */
  virtual void on_wires( ast_id id, const std::vector<ast_id>& wires ) const
  {
    (void)id;
    (void)wires;
  }

  virtual void on_module_instantiation( ast_id id,
                                        const std::string& module_name,
                                        const std::vector<ast_id>& params,
                                        const std::string& instance_name,
                                        const std::vector<std::pair<std::string, ast_id>>& args ) const
  {
    (void)id;
    (void)module_name;
    (void)params;
    (void)instance_name;
    (void)args;
  }
}; // verilog_reader_visitor

} // namespace lorina
