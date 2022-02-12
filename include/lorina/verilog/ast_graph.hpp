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

  inline ast_id create_input_declaration( ast_id identifier_list )
  {
    assert( identifier_list < nodes_.size() );
    if ( const ast_identifier_list* node = dynamic_cast<ast_identifier_list*>( nodes_[identifier_list] ) )
    {
      return create_node<ast_input_declaration>( node->identifiers() );
    }
    else if ( const ast_identifier* node = dynamic_cast<ast_identifier*>( nodes_[identifier_list] ) )
    {
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
      return create_node<ast_input_declaration>( node->identifiers(), range_expr->hi(), range_expr->lo() );
    }
    else if ( const ast_identifier* node = dynamic_cast<ast_identifier*>( nodes_[identifier_list] ) )
    {
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
      return create_node<ast_output_declaration>( node->identifiers() );
    }
    else if ( const ast_identifier* node = dynamic_cast<ast_identifier*>( nodes_[identifier_list] ) )
    {
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
      return create_node<ast_output_declaration>( node->identifiers(), range_expr->hi(), range_expr->lo() );
    }
    else if ( const ast_identifier* node = dynamic_cast<ast_identifier*>( nodes_[identifier_list] ) )
    {
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
      return create_node<ast_wire_declaration>( node->identifiers() );
    }
    else if ( const ast_identifier* node = dynamic_cast<ast_identifier*>( nodes_[identifier_list] ) )
    {
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
      return create_node<ast_wire_declaration>( node->identifiers(), range_expr->hi(), range_expr->lo() );
    }
    else if ( const ast_identifier* node = dynamic_cast<ast_identifier*>( nodes_[identifier_list] ) )
    {
      return create_node<ast_wire_declaration>( std::vector<ast_id>( identifier_list ), range_expr->hi(), range_expr->lo() );
    }
    else
    {
      assert( false && "unknown node type" );
      std::abort();
    }
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
