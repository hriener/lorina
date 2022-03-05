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
  \file ast_topo_visitor.hpp
  \brief Traverses the AST in topological order and invokes the
         callbacks.

  \author Heinz Riener
*/

#pragma once

#include "context.hpp"
#include "reader_visitor.hpp"
#include "ast_graph.hpp"
#include "ast_stringifier.hpp"
#include <fmt/format.h>
#include <unordered_set>
#include <memory>

namespace lorina
{

namespace detail
{

class leaf_collector_visitor : public verilog_ast_visitor
{
public:
  explicit leaf_collector_visitor( const verilog_ast_graph* ag )
    : verilog_ast_visitor( ag )
    , stringifier_( ag )
  {
  }

  void visit( const ast_numeral& node )
  {
    node.accept( stringifier_ );
    // emplace_leaves( &node, { stringifier_.string_of( node ) } );
    emplace_leaves( &node, {} );
  }

  void visit( const ast_identifier& node )
  {
    node.accept( stringifier_ );
    emplace_leaves( &node, { stringifier_.string_of( node ) } );
  }

  void visit( const ast_arithmetic_identifier& node )
  {
    node.accept( stringifier_ );
    emplace_leaves( &node, { stringifier_.string_of( node ) } );
  }

  void visit( const ast_input_declaration& node )
  {
    emplace_leaves( &node, collect_leaves_of_children( node ) );
  }

  void visit( const ast_output_declaration& node )
  {
    emplace_leaves( &node, collect_leaves_of_children( node ) );
  }

  void visit( const ast_wire_declaration& node )
  {
    emplace_leaves( &node, collect_leaves_of_children( node ) );
  }

  void visit( const ast_expression& node )
  {
    emplace_leaves( &node, collect_leaves_of_children( node ) );
  }

  void visit( const ast_system_function& node )
  {
    emplace_leaves( &node, collect_leaves_of_children( node ) );
  }

  std::vector<std::string> leaves( const ast_node* node ) const
  {
    return leaves_.at( node );
  }

protected:
  std::vector<std::string> collect_leaves_of_children( const ast_node& node )
  {
    std::vector<std::string> leaves;
    node.foreach_child( [&]( const ast_id id ){
      const ast_node* child = ag_->node_ptr( id );
      child->accept( *this );
      for ( const auto& l : leaves_[child] )
      {
        if ( std::find( std::begin( leaves ), std::end( leaves ), l ) == std::end( leaves ) )
        {
          leaves.emplace_back( l );
        }
      }
    });
    return leaves;
  }

  void emplace_leaves( const ast_node* node, const std::vector<std::string>& a )
  {
    std::vector<std::string> vs;
    for ( const std::string& s : a )
    {
      if ( std::find( std::begin( vs ), std::end( vs ), s ) == std::end( vs ) )
      {
        vs.emplace_back( s );
      }
    }
    leaves_.emplace( node, vs );
  }

protected:
  ast_stringifier stringifier_;
  std::unordered_map<const ast_node*, std::vector<std::string>> leaves_;
}; // leaf_collector_visitor

} /* detail */

class verilog_ast_topological_visitor : public verilog_ast_visitor
{
public:
  explicit verilog_ast_topological_visitor( const verilog_ast_graph* ag, verilog_reader_visitor& vis, verilog_context* ctx )
    : verilog_ast_visitor( ag )
    , leaf_collector_( ag )
    , ctx_( ctx )
    , reader_( vis )
  {
  }

  /* invoke callbacks */
  void visit( const ast_numeral& node )
  {
    reader_.on_constant( node.id(), node.value() );
  }

  void visit( const ast_identifier& node )
  {
    reader_.on_identifier( node.id(), node.identifier() );
  }

  void visit( const ast_arithmetic_identifier& node )
  {
    reader_.on_arithmetic_identifier( node.id(), node.identifier() );
  }

  void visit( const ast_expression& node )
  {
    const expr_kind kind = node.kind();
    switch ( kind )
    {
    case expr_kind::EXPR_NOT:
      {
        const ast_node* left = ag_->node_ptr( node.left() );
        left->accept( *this );
        reader_.on_not( node.id(), node.left() );
      }
      break;
    case expr_kind::EXPR_AND:
      {
        const ast_node* left = ag_->node_ptr( node.left() );
        const ast_node* right = ag_->node_ptr( node.right() );
        left->accept( *this );
        right->accept( *this );
        reader_.on_and( node.id(), node.left(), node.right() );
      }
      break;
    case expr_kind::EXPR_OR:
      {
        const ast_node* left = ag_->node_ptr( node.left() );
        const ast_node* right = ag_->node_ptr( node.right() );
        left->accept( *this );
        right->accept( *this );
        reader_.on_or( node.id(), node.left(), node.right() );
      }
      break;
    case expr_kind::EXPR_XOR:
      {
        const ast_node* left = ag_->node_ptr( node.left() );
        const ast_node* right = ag_->node_ptr( node.right() );
        left->accept( *this );
        right->accept( *this );
        reader_.on_xor( node.id(), node.left(), node.right() );
      }
      break;
    case expr_kind::EXPR_ADD:
      {
        const ast_node* left = ag_->node_ptr( node.left() );
        const ast_node* right = ag_->node_ptr( node.right() );
        left->accept( *this );
        right->accept( *this );
        reader_.on_add( node.id(), node.left(), node.right() );
      }
      break;
    case expr_kind::EXPR_MUL:
      {
        const ast_node* left = ag_->node_ptr( node.left() );
        const ast_node* right = ag_->node_ptr( node.right() );
        left->accept( *this );
        right->accept( *this );
        reader_.on_sub( node.id(), node.left(), node.right() );
      }
      break;
    default:
      assert( false );
    }
  }

  void visit( const ast_system_function& node )
  {
    const ast_node* function = ag_->node_ptr( node.fun() );
    function->accept( leaf_collector_ );

    for ( const ast_id id : node.args() )
    {
      const ast_node* arg = ag_->node_ptr( id );
      arg->accept( *this );
    }
    reader_.on_system_function( node.id(), leaf_collector_.leaves(function)[0], node.args() );
  }

  /* process declarations and statements */
  void visit( const ast_input_declaration& node )
  {
    /* declare all children known */
    for ( const ast_id& id : node.identifiers() )
    {
      const ast_node* identifier = ag_->node_ptr( id );
      identifier->accept( leaf_collector_ );

      for ( const std::string& def : leaf_collector_.leaves( identifier ) )
      {
        declare_known( def );
      }
    }

    for ( const ast_id& id : node.identifiers() )
    {
      const ast_node* identifier = ag_->node_ptr( id );
      identifier->accept( *this );
    }

    reader_.on_inputs( node.id(), node.identifiers() );
  }

  void visit( const ast_output_declaration& node )
  {
    for ( const ast_id& id : node.identifiers() )
    {
      const ast_node* identifier = ag_->node_ptr( id );
      identifier->accept( *this );
    }
    reader_.on_outputs( node.id(), node.identifiers() );
  }

  void visit( const ast_wire_declaration& node )
  {
    for ( const ast_id& id : node.identifiers() )
    {
      const ast_node* identifier = ag_->node_ptr( id );
      identifier->accept( *this );
    }
    reader_.on_wires( node.id(), node.identifiers() );
  }

  void visit( const ast_assignment& node )
  {
    const ast_node* signal = ag_->node_ptr( node.signal() );
    const ast_node* expr = ag_->node_ptr( node.expr() );
    signal->accept( leaf_collector_ );
    expr->accept( leaf_collector_ );

    const auto& defs = leaf_collector_.leaves( signal );
    const auto& users = leaf_collector_.leaves( expr );

    /* test if all users are already known */
    std::unordered_set<std::string> waitees;
    for ( const std::string& user : users )
    {
      if ( !is_known( user ) )
      {
        waitees.emplace( user );
      }
    }

    /* if not, then proceed with another node */
    if ( waitees.size() > 0 )
    {
      waits_for_.emplace( &node, waitees );
      return;
    }

    signal->accept( *this );
    expr->accept( *this );

    reader_.on_assignment( node.id(), node.signal(), node.expr() );

    /* declare defs known */
    declare_known( defs );

    /* remove from waitees and retrigger waiting nodes */
    trigger( defs );
  }

  void visit( const ast_parameter_declaration& node )
  {
    const ast_node* identifier = ag_->node_ptr( node.identifier() );
    const ast_node* expr = ag_->node_ptr( node.expr() );
    identifier->accept( leaf_collector_ );
    expr->accept( leaf_collector_ );

    const auto& defs = leaf_collector_.leaves( identifier );
    const auto& users = leaf_collector_.leaves( expr );

    /* test if all users are already known */
    std::unordered_set<std::string> waitees;
    for ( const std::string& user : users )
    {
      if ( !is_known( user ) )
      {
        waitees.emplace( user );
      }
    }

    /* if not, then proceed with another node */
    if ( waitees.size() > 0 )
    {
      waits_for_.emplace( &node, waitees );
      return;
    }

    identifier->accept( *this );
    expr->accept( *this );

    reader_.on_parameter( node.id(), node.identifier(), node.expr() );

    /* declare defs known */
    declare_known( defs );

    /* remove from waitees and retrigger waiting nodes */
    trigger( defs );
  }

  void visit( const ast_module& node )
  {
    const std::string& module_name = node.module_name();

    std::vector<std::string> inouts;
    for ( ast_id arg : node.args() )
    {
      const ast_node* arg_node = ag_->node_ptr( arg );
      arg_node->accept( leaf_collector_ );
      for ( const std::string& inout : leaf_collector_.leaves( arg_node ) )
      {
        inouts.emplace_back( inout );
      }
    }

    reader_.on_module_header( node.id(), module_name, inouts );

    for ( ast_id decl : node.decls() )
    {
      const ast_node* decl_node = ag_->node_ptr( decl );
      decl_node->accept( *this );
    }
  }

  void visit( const ast_module_instantiation& node )
  {
    if ( ctx_ )
    {
      /* find the declaration of the module */
      const ast_node* module_name = ag_->node_ptr( node.module_name() );
      module_name->accept( leaf_collector_ );

      const ast_node* inst_name = ag_->node_ptr( node.instance_name() );
      inst_name->accept( leaf_collector_ );

      const std::string name = leaf_collector_.leaves( module_name )[0];
      std::shared_ptr<module> m = ctx_->find( name );
      if ( !m.get() )
      {
        std::cout << "unknown module" << std::endl;
        assert( false );
      }

      /* compute def and use */
      std::vector<std::pair<std::string, ast_id>> assignment;

      std::vector<std::string> defs;
      std::vector<std::string> users;
      for ( const std::pair<ast_id,ast_id>& p : node.port_assignment() )
      {
        const ast_node* lhs = ag_->node_ptr( p.first );
        const ast_node* rhs = ag_->node_ptr( p.second );
        lhs->accept( leaf_collector_ );
        rhs->accept( leaf_collector_ );

        assert( leaf_collector_.leaves( lhs ).size() == 1 );
        const std::string port_name = leaf_collector_.leaves( lhs )[0];
        if ( m->has_input( port_name ) )
        {
          for ( const std::string& s : leaf_collector_.leaves( rhs ) )
          {
            if ( std::find( std::begin( users ), std::end( users ), s ) == std::end( users ) )
            {
              users.emplace_back( s );
            }
          }
          assignment.emplace_back( std::make_pair( port_name, p.second ) );
        }
        else if ( m->has_output( port_name ) )
        {
          for ( const std::string& s : leaf_collector_.leaves( rhs ) )
          {
            if ( std::find( std::begin( defs ), std::end( defs ), s ) == std::end( defs ) )
            {
              defs.emplace_back( s );
            }
          }
          assignment.emplace_back( std::make_pair( port_name, p.second ) );
        }
        else
        {
          std::cout << "unknown port name: " << port_name << std::endl;
          assert( false );
        }
      }

      for ( const ast_id id : node.parameters() )
      {
        const ast_node* param_node = ag_->node_ptr( id );
        param_node->accept( *this );

        for ( const std::string& s : leaf_collector_.leaves( param_node ) )
        {
          if ( std::find( std::begin( users ), std::end( users ), s ) == std::end( users ) )
          {
            users.emplace_back( s );
          }
        }
      }

      /* test if all users are already known */
      std::unordered_set<std::string> waitees;
      for ( const std::string& user : users )
      {
        if ( !is_known( user ) )
        {
          waitees.emplace( user );
        }
      }

      /* if not, then proceed with another node */
      if ( waitees.size() > 0 )
      {
        waits_for_.emplace( &node, waitees );
        return;
      }

      reader_.on_module_instantiation( node.id(), leaf_collector_.leaves( module_name )[0], node.parameters(), leaf_collector_.leaves( inst_name )[0], assignment );

      /* declare defs known */
      declare_known( defs );

      /* remove from waitees and retrigger waiting nodes */
      trigger( defs );
    }
    else
    {
      assert( false && "unknown module" );
    }
  }

protected:
  void declare_known( const std::string& name )
  {
    known_.emplace( name );
  }

  void declare_known( const std::vector<std::string>& names )
  {
    for ( const std::string& name : names )
    {
      known_.emplace( name );
    }
  }

  bool is_known( const std::string& name ) const
  {
    return known_.find( name ) != std::end( known_ );
  }

  void trigger( const std::vector<std::string>& defs )
  {
    std::vector<decltype(waits_for_)::key_type> to_remove;
    for ( const std::string& def : defs )
    {
      for ( auto&& entry : waits_for_ )
      {
        entry.second.erase( def );
        if ( entry.second.empty() )
        {
          to_remove.emplace_back( entry.first );
        }
      }
    }

    for ( auto&& key : to_remove )
    {
      waits_for_.erase(key);
    }

    /* finally trigger the node */
    for ( auto&& key :  to_remove )
    {
      key->accept( *this );
    }
  }

protected:
  detail::leaf_collector_visitor leaf_collector_;
  verilog_context* ctx_;
  verilog_reader_visitor& reader_;

  std::unordered_set<std::string> known_;
  std::unordered_map<const ast_node*, std::unordered_set<std::string>> waits_for_;
};

} // namespace lorina
