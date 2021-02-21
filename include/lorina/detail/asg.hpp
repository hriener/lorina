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
  \file asg.hpp
  \brief Abstract syntax graph (ASG)

  \author Heinz Riener
*/

#pragma once

#include <vector>
#include <assert>

namespace lorina
{

/*! \brief Abstract syntax graph
 *
 * The type of a node is passed as a template argument.  Nodes are identified by indices.
 *
 * Sharing of subgraphs is allowed such that the graph is directed and
 * acyclic, and does not necessarily has a tree structure.
 * Consequently, we call the data structure ``abstract syntax graph''
 * (ASG) instead of ``abstract syntax tree'' (AST).
 */
template<typename node_type>
class asg
{
public:
  using index_type = uint32_t;

public:
  explicit asg() {}

  /* \brief Create a new node */
  index_type create_node( node_type const& n )
  {
    index_type const index = nodes.size();
    nodes.emplace_back( n );
    return index;
  }

  /* \brief Get node at index */
  node_type& operator[]( index_type index )
  {
    assert( index < nodes.size() && "out-of-bounds access" );
    return nodes[index];
  }

  /* \brief Get node at index */
  node_type const& operator[]( index_type index ) const
  {
    assert( index < nodes.size() && "out of bounds access" );
    return nodes[index];
  }

  /* \brief Call a functor on each node */
  template<typename Fn>
  void foreach_node( Fn&& fn ) const
  {
    for ( const auto& n : nodes )
    {
      fn( n );
    }
  }

  /* \brief Call a functor on each node
   *
   * The mutable methods use index-access instead of iterator because
   * iterators may get invalidated if new nodes are added while
   * iterating.
   */
  template<typename Fn>
  void foreach_node( Fn&& fn )
  {
    for ( auto i = 0ul; i < nodes.size(); ++i )
    {
      fn( nodes[i] );
    }
  }

  /* \brief Returns the number of nodes
   *
   */
  uint64_t num_nodes() const
  {
    return nodes.size();
  }

protected:
  std::vector<node_type> nodes;
}; /* asg */

} /* lorina */
