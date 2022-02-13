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
  \file ast_graph.hpp
  \brief Verilog AST Graph

  \author Heinz Riener
*/

#pragma once

namespace lorina
{

using ast_id = uint32_t;

class ast_node
{
public:
  explicit ast_node() = default;

  virtual ~ast_node() {}
}; // ast_node

/* \brief Numeral
 *
 * A numeral.
 *
 */
class ast_numeral : public ast_node
{
public:
  explicit ast_numeral( const std::string value )
    : value_( value )
  {
  }

  inline std::string value() const
  {
    return value_;
  }

protected:
  std::string value_;
}; // ast_numeral

/* \brief Identifier
 *
 * An identifier.
 *
 */
class ast_identifier : public ast_node
{
public:
  explicit ast_identifier( const std::string identifier )
    : identifier_( identifier )
  {
  }

  inline std::string identifier() const
  {
    return identifier_;
  }

protected:
  std::string identifier_;
}; // ast_identifier

/* \brief Identifier list
 *
 * A list of identifiers of form
 *   IDENTIFIER `,` ... `,` IDENTIFIER
 *
 */
class ast_identifier_list : public ast_node
{
public:
  explicit ast_identifier_list( const std::vector<ast_id>& identifiers )
    : identifiers_( identifiers )
  {
  }

  inline std::vector<ast_id> identifiers() const
  {
    return identifiers_;
  }

protected:
  std::vector<ast_id> identifiers_;
}; // ast_identifier_list

/* \brief Array select
 *
 * An identifier followed by an bit selector of form
 *   IDENTIFIER `[` NUMERAL `]`
 *
 */
class ast_array_select : public ast_node
{
public:
  explicit ast_array_select( ast_id array, ast_id index )
    : array_( array )
    , index_( index )
  {
  }

  inline ast_id array() const
  {
    return array_;
  }

  inline ast_id index() const
  {
    return index_;
  }

protected:
  ast_id array_;
  ast_id index_;
}; // ast_array_select

/* \brief Range expression
 *
 * A pair of a most-significant bit and a least-significant bit of form
 *   `[` MSB `:` LSB `]`
 *
 */
class ast_range_expression : public ast_node
{
public:
  explicit ast_range_expression( ast_id hi, ast_id lo )
    : hi_( hi )
    , lo_( lo )
  {
  }

  inline ast_id hi() const
  {
    return hi_;
  }

  inline ast_id lo() const
  {
    return lo_;
  }

protected:
  ast_id hi_;
  ast_id lo_;
};

enum class sign_kind
{
  SIGN_MINUS = 1,
};

/* \brief Sign
 *
 */
class ast_sign : public ast_node
{
public:
  explicit ast_sign( sign_kind kind, ast_id expr )
    : kind_( kind )
    , expr_( expr )
  {}

  inline ast_id expr() const
  {
    return expr_;
  }

  inline sign_kind kind() const
  {
    return kind_;
  }

protected:
  sign_kind kind_;
  ast_id expr_;
}; // ast_sign

enum class expr_kind
{
  EXPR_ADD = 1,
  EXPR_MUL = 2,
  EXPR_NOT = 3,
  EXPR_AND = 4,
  EXPR_OR = 5,
  EXPR_XOR = 6,
};

/* \brief Expression
 *
 */
class ast_expression : public ast_node
{
public:
  explicit ast_expression( expr_kind kind, ast_id left )
    : kind_( kind )
    , args_( {left} )
  {}

  explicit ast_expression( expr_kind kind, ast_id left, ast_id right )
    : kind_( kind )
    , args_( {left, right} )
  {
  }

  inline ast_id left() const
  {
    assert( args_.size() >= 1 );
    return args_[0];
  }

  inline ast_id right() const
  {
    assert( args_.size() >= 2 );
    return args_[1];
  }

  inline expr_kind kind() const
  {
    return kind_;
  }

protected:
  expr_kind kind_;
  std::vector<ast_id> args_;
};

/* \brief System function
 *
 */
class ast_system_function : public ast_node
{
public:
  explicit ast_system_function( ast_id fun, const std::vector<ast_id>& args )
    : fun_( fun )
    , args_( args )
  {
  }

  inline ast_id fun() const
  {
    return fun_;
  }

protected:
  const ast_id fun_;
  const std::vector<ast_id> args_;
};

/* \brief Input declaration
 *
 * An input declaration of form
 *   `input` ( `[` NUMERAL `:` NUMERAL `]` )? IDENTIFIER `,` ... `,` IDENTIFIER `;`
 *
 */
class ast_input_declaration : public ast_node
{
public:
  explicit ast_input_declaration( const std::vector<ast_id>& identifiers )
    : word_level_( false )
    , identifiers_( identifiers )
  {
  }

  explicit ast_input_declaration( const std::vector<ast_id>& identifiers, ast_id hi, ast_id lo )
    : word_level_( true )
    , identifiers_( identifiers )
    , hi_( hi )
    , lo_( lo )
  {
  }

  inline std::vector<ast_id> identifiers() const
  {
    return identifiers_;
  }

  inline ast_id hi() const
  {
    assert( word_level_ );
    return hi_;
  }

  inline ast_id lo() const
  {
    assert( word_level_ );
    return lo_;
  }

protected:
  bool word_level_;
  std::vector<ast_id> identifiers_;
  ast_id hi_;
  ast_id lo_;
}; // ast_input_declaration

/* \brief Output declaration
 *
 * An output declaration of form
 *   `output` ( `[` NUMERAL `:` NUMERAL `]` )? IDENTIFIER `,` ... `,` IDENTIFIER `;`
 *
 */
class ast_output_declaration : public ast_node
{
public:
  explicit ast_output_declaration( const std::vector<ast_id>& identifiers )
    : word_level_( false )
    , identifiers_( identifiers )
  {
  }

  explicit ast_output_declaration( const std::vector<ast_id>& identifiers, ast_id hi, ast_id lo )
    : word_level_( true )
    , identifiers_( identifiers )
    , hi_( hi )
    , lo_( lo )
  {
  }

  inline std::vector<ast_id> identifiers() const
  {
    return identifiers_;
  }

  inline ast_id hi() const
  {
    assert( word_level_ );
    return hi_;
  }

  inline ast_id lo() const
  {
    assert( word_level_ );
    return lo_;
  }

protected:
  bool word_level_;
  std::vector<ast_id> identifiers_;
  ast_id hi_;
  ast_id lo_;
}; // ast_output_declaration

/* \brief Wire declaration
 *
 * An wire declaration of form
 *   `wire` ( `[` NUMERAL `:` NUMERAL `]` )? IDENTIFIER `,` ... `,` IDENTIFIER `;`
 *
 */
class ast_wire_declaration : public ast_node
{
public:
  explicit ast_wire_declaration( const std::vector<ast_id>& identifiers )
    : word_level_( false )
    , identifiers_( identifiers )
  {
  }

  explicit ast_wire_declaration( const std::vector<ast_id>& identifiers, ast_id hi, ast_id lo )
    : word_level_( true )
    , identifiers_( identifiers )
    , hi_( hi )
    , lo_( lo )
  {
  }

  inline std::vector<ast_id> identifiers() const
  {
    return identifiers_;
  }

  inline ast_id hi() const
  {
    assert( word_level_ );
    return hi_;
  }

  inline ast_id lo() const
  {
    assert( word_level_ );
    return lo_;
  }

protected:
  bool word_level_;
  std::vector<ast_id> identifiers_;
  ast_id hi_;
  ast_id lo_;
}; // ast_wire_declaration

/* \brief Module instantiation
 *
 * A module instantiation of form
 *   IDENTIFIER (ParameterAssignment)? IDENTIFIER `(` PortAssignment `)` `;`
 * with
 *   ParameterAssignment ::= `#` `(` ARITH_EXPR `,` ... `,` ARITH_EXPR `)`
 *   PortAssignment ::= `.` IDENTIFIER(SIGNAL_REFERENCE) `,` ... `,` `.` IDENTIFIER(SIGNAL_REFERENCE).
 */
class ast_module_instantiation : public ast_node
{
public:
  explicit ast_module_instantiation( ast_id module_name, ast_id instance_name,
                                     const std::vector<std::pair<ast_id, ast_id>>& port_assignment,
                                     const std::vector<ast_id>& parameters )
    : module_name_( module_name )
    , instance_name_( instance_name )
    , port_assignment_( port_assignment )
    , parameters_( parameters )
  {
  }

protected:
  ast_id module_name_;
  ast_id instance_name_;
  std::vector<std::pair<ast_id, ast_id>> port_assignment_;
  std::vector<ast_id> parameters_;
}; // ast_module_instantiation

/* \brief Parameter declaration
 *
 */
class ast_parameter_declaration : public ast_node
{
public:
  explicit ast_parameter_declaration( ast_id identifier, ast_id expr )
    : identifier_( identifier )
    , expr_( expr )
  {
  }

protected:
  ast_id identifier_;
  ast_id expr_;
}; // ast_parameter_declaration

/* \brief Assignment statement
 *
 */
class ast_assignment : public ast_node
{
public:
  explicit ast_assignment( ast_id signal, ast_id expr )
    : signal_( signal )
    , expr_( expr )
  {
  }

protected:
  ast_id signal_;
  ast_id expr_;
}; // ast_assignment

/* \brief Module
 *
 */
class ast_module : public ast_node
{
public:
  explicit ast_module( ast_id module_name, const std::vector<ast_id>& args, const std::vector<ast_id>& decls )
    : module_name_( module_name )
    , args_( args )
    , decls_( decls )
  {
  }

protected:
  ast_id module_name_;
  std::vector<ast_id> args_;
  std::vector<ast_id> decls_;
}; // ast_module

class verilog_ast_graph
{
public:
  class ast_or_error
  {
  public:
    explicit ast_or_error( ast_id ast )
      : ast_( ast )
    {
      ast_ |= 0x80000000;
    }

    explicit ast_or_error()
      : ast_( 0 )
    {
    }

    inline ast_id ast() const
    {
      return ast_ & 0x7FFFFFFF;
    }

    inline operator bool() const
    {
      return valid();
    }

    inline bool valid() const
    {
      return ast_ & 0x80000000;
    }

  protected:
    ast_id ast_;
  };

public:
  explicit verilog_ast_graph() = default;

  virtual ~verilog_ast_graph()
  {
    cleanup();
  }

  inline ast_id create_numeral( const std::string& numeral )
  {
    return create_node<ast_numeral>( numeral );
  }

  inline ast_id create_identifier( const std::string& identifier )
  {
    return create_node<ast_identifier>( identifier );
  }

  inline ast_id create_identifier_list( const std::vector<ast_id>& identifier_list )
  {
    return create_node<ast_identifier_list>( identifier_list );
  }

  inline ast_id create_range_expression( ast_id hi, ast_id lo )
  {
    return create_node<ast_range_expression>( hi, lo );
  }

  inline ast_id create_array_select( ast_id array, ast_id index )
  {
    return create_node<ast_array_select>( array, index );
  }

  inline ast_id create_sum_expression( ast_id term, ast_id expr )
  {
    return create_node<ast_expression>( expr_kind::EXPR_ADD, term, expr );
  }

  inline ast_id create_negative_sign( ast_id expr )
  {
    return create_node<ast_sign>( sign_kind::SIGN_MINUS, expr );
  }

  inline ast_id create_mul_expression( ast_id term, ast_id expr )
  {
    return create_node<ast_expression>( expr_kind::EXPR_MUL, term, expr );
  }

  inline ast_id create_not_expression( ast_id expr )
  {
    return create_node<ast_expression>( expr_kind::EXPR_NOT, expr );
  }

  inline ast_id create_and_expression( ast_id term, ast_id expr )
  {
    return create_node<ast_expression>( expr_kind::EXPR_AND, term, expr );
  }

  inline ast_id create_or_expression( ast_id term, ast_id expr )
  {
    return create_node<ast_expression>( expr_kind::EXPR_OR, term, expr );
  }

  inline ast_id create_xor_expression( ast_id term, ast_id expr )
  {
    return create_node<ast_expression>( expr_kind::EXPR_XOR, term, expr );
  }

  inline ast_id create_system_function( ast_id fun, const std::vector<ast_id>& args )
  {
    return create_node<ast_system_function>( fun, args );
  }

  inline ast_id create_input_declaration( ast_id identifier_list )
  {
    assert( identifier_list < nodes_.size() );
    if ( const ast_identifier_list* node = dynamic_cast<ast_identifier_list*>( nodes_[identifier_list] ) )
    {
      (void)node;
      return create_node<ast_input_declaration>( node->identifiers() );
    }
    else if ( const ast_identifier* node = dynamic_cast<ast_identifier*>( nodes_[identifier_list] ) )
    {
      (void)node;
      return create_node<ast_input_declaration>( std::vector<ast_id>( identifier_list ) );
    }
    else
    {
      assert( false && "unknown node type" );
      std::abort();
    }
  }

  inline ast_id create_input_declaration( ast_id identifier_list, ast_id range )
  {
    assert( identifier_list < nodes_.size() );
    assert( range < nodes_.size() );
    const ast_range_expression* range_expr = static_cast<ast_range_expression*>( nodes_[range] );
    if ( const ast_identifier_list* node = dynamic_cast<ast_identifier_list*>( nodes_[identifier_list] ) )
    {
      (void)node;
      return create_node<ast_input_declaration>( node->identifiers(), range_expr->hi(), range_expr->lo() );
    }
    else if ( const ast_identifier* node = dynamic_cast<ast_identifier*>( nodes_[identifier_list] ) )
    {
      (void)node;
      return create_node<ast_input_declaration>( std::vector<ast_id>( identifier_list ), range_expr->hi(), range_expr->lo() );
    }
    else
    {
      assert( false && "unknown node type" );
      std::abort();
    }
  }

  inline ast_id create_output_declaration( ast_id identifier_list )
  {
    assert( identifier_list < nodes_.size() );
    if ( const ast_identifier_list* node = dynamic_cast<ast_identifier_list*>( nodes_[identifier_list] ) )
    {
      (void)node;
      return create_node<ast_output_declaration>( node->identifiers() );
    }
    else if ( const ast_identifier* node = dynamic_cast<ast_identifier*>( nodes_[identifier_list] ) )
    {
      (void)node;
      return create_node<ast_output_declaration>( std::vector<ast_id>( identifier_list ) );
    }
    else
    {
      assert( false && "unknown node type" );
      std::abort();
    }
  }

  inline ast_id create_output_declaration( ast_id identifier_list, ast_id range )
  {
    assert( identifier_list < nodes_.size() );
    assert( range < nodes_.size() );
    const ast_range_expression* range_expr = static_cast<ast_range_expression*>( nodes_[range] );
    if ( const ast_identifier_list* node = dynamic_cast<ast_identifier_list*>( nodes_[identifier_list] ) )
    {
      (void)node;
      return create_node<ast_output_declaration>( node->identifiers(), range_expr->hi(), range_expr->lo() );
    }
    else if ( const ast_identifier* node = dynamic_cast<ast_identifier*>( nodes_[identifier_list] ) )
    {
      (void)node;
      return create_node<ast_output_declaration>( std::vector<ast_id>( identifier_list ), range_expr->hi(), range_expr->lo() );
    }
    else
    {
      assert( false && "unknown node type" );
      std::abort();
    }
  }

  inline ast_id create_wire_declaration( ast_id identifier_list )
  {
    assert( identifier_list < nodes_.size() );
    if ( const ast_identifier_list* node = dynamic_cast<ast_identifier_list*>( nodes_[identifier_list] ) )
    {
      (void)node;
      return create_node<ast_wire_declaration>( node->identifiers() );
    }
    else if ( const ast_identifier* node = dynamic_cast<ast_identifier*>( nodes_[identifier_list] ) )
    {
      (void)node;
      return create_node<ast_wire_declaration>( std::vector<ast_id>( identifier_list ) );
    }
    else
    {
      assert( false && "unknown node type" );
      std::abort();
    }
  }

  inline ast_id create_wire_declaration( ast_id identifier_list, ast_id range )
  {
    assert( identifier_list < nodes_.size() );
    assert( range < nodes_.size() );
    const ast_range_expression* range_expr = static_cast<ast_range_expression*>( nodes_[range] );
    if ( const ast_identifier_list* node = dynamic_cast<ast_identifier_list*>( nodes_[identifier_list] ) )
    {
      (void)node;
      return create_node<ast_wire_declaration>( node->identifiers(), range_expr->hi(), range_expr->lo() );
    }
    else if ( const ast_identifier* node = dynamic_cast<ast_identifier*>( nodes_[identifier_list] ) )
    {
      (void)node;
      return create_node<ast_wire_declaration>( std::vector<ast_id>( identifier_list ), range_expr->hi(), range_expr->lo() );
    }
    else
    {
      assert( false && "unknown node type" );
      std::abort();
    }
  }

  inline ast_id create_module_instantiation( ast_id module_name, ast_id instance_name,
                                             const std::vector<std::pair<ast_id, ast_id>>& port_assignment,
                                             const std::vector<ast_id>& parameters )
  {
    return create_node<ast_module_instantiation>( module_name, instance_name, port_assignment, parameters );
  }

  inline ast_id create_parameter_declaration( ast_id identifier, ast_id expr )
  {
    return create_node<ast_parameter_declaration>( identifier, expr );
  }

  inline ast_id create_assignment( ast_id signal, ast_id expr )
  {
    return create_node<ast_assignment>( signal, expr );
  }

  inline ast_id create_module( ast_id module_name, const std::vector<ast_id>& args, const std::vector<ast_id>& decls )
  {
    return create_node<ast_module>( module_name, args, decls );
  }

protected:
  template<typename T, typename... Args>
  inline ast_id create_node( Args... args )
  {
    const uint32_t index = nodes_.size();
    nodes_.emplace_back( new T( args... ) );
    return index;
  }

protected:
  inline void cleanup()
  {
    for ( auto& node : nodes_ )
    {
      delete node;
    }
  }

protected:
  std::vector<ast_node*> nodes_;
}; // verilog_ast_graph

} // namespace lorina
