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
  \file smt2.hpp
  \brief Implements SMT2 parser

  \author Heinz Riener
*/

#pragma once

#include "detail/utils.hpp"

#include <iostream>
#include <istream>
#include <string>
#include <optional>
#include <vector>

namespace lorina
{

template<typename ASG>
class smt2_reader
{
public:
  using asg_type = ASG;
  using index_type = typename ASG::index_type;

public:
  explicit smt2_reader( ASG& g )
    : g( g )
  {}

  virtual void on_top( index_type id ) const
  {
    (void)id;
  }

  virtual void on_preorder( index_type id ) const
  {
    (void)id;
  }

  virtual void on_postorder( index_type id ) const
  {
    (void)id;
  }

  virtual void on_leave( index_type id ) const
  {
    (void)id;
  }

  virtual void on_child( index_type id ) const
  {
    (void)id;
  }

  virtual void on_end( index_type id ) const
  {
    (void)id;
  }

public:
  ASG& g;
}; /* smt2_reader */

template<typename ASG>
class smt2_pretty_printer : public smt2_reader<ASG>
{
public:
  using asg_type = ASG;
  using index_type = typename ASG::index_type;

public:
  explicit smt2_pretty_printer( ASG& g, std::ostream& os = std::cout )
    : smt2_reader<ASG>( g )
    , os( os )
  {}

  void on_top( index_type id ) const override
  {
    (void)id;
  }

  void on_preorder( index_type id ) const override
  {
    os << "(" << this->g[id].data;
  }

  void on_postorder( index_type id ) const override
  {
    (void)id;
    os << ")";
  }

  void on_leave( index_type id ) const override
  {
    os << this->g[id].data;
  }

  void on_child( index_type id ) const override
  {
    (void)id;
    os << ' ';
  }

  void on_end( index_type id ) const override
  {
    (void)id;
    os << '\n';
    os.flush();
  }

public:
  std::ostream& os;
}; /* smt2_pretty_printer */

/*! \brief SNode
 *
 */
struct snode
{
  snode( std::string data )
    : data( data )
    , children( {} )
  {
  }

  snode( std::string data, std::vector<uint32_t> const& children )
    : data( data )
    , children( children )
  {
  }

  std::string data;
  std::vector<uint32_t> children;
};

template<typename Reader>
class sexpr_parser
{
public:
  using asg_type = typename Reader::asg_type;
  using index_type = typename asg_type::index_type;

  enum class token_kind
  {
    tok_param,
    tok_break,
    tok_string,
    tok_keyword,
  };

  struct token
  {
    explicit token() {}

    explicit token( token_kind kind )
      : kind( kind )
      , value( {} )
    {
    }

    explicit token( token_kind kind, std::string const& value )
      : kind( kind )
      , value( value )
    {
    }

    token_kind kind;
    std::string value;
  };

public:
  explicit sexpr_parser( std::istream& in, Reader const& reader )
    : in( in )
    , reader( reader )
  {}

  token get_token()
  {
    std::string tok{};
    while ( !in.eof() )
    {
      char const ch = in.get();

      if ( ch == std::char_traits<char>::eof() )
      {
        break;
      }
      else if ( tok.size() == 0 && ( ch == '(' || ch == ')' ) )
      {
        return token{token_kind::tok_param, std::string{ch}};
      }
      else if ( tok.size() == 0 && ( ch == '\n' ) )
      {
        return token{token_kind::tok_break};
      }
      else if ( tok.size() == 0 && ( ch == ' ' ) )
      {
        continue; // consume whitespace
      }
      else if ( ch == ' ' | ch == '\n' | ch == '(' | ch == ')' )
      {
        in.unget();
        break;
      }
      else
      {
        tok += ch; // otherwise concat chars
      }
    }

    detail::trim( tok );
    if ( tok == "set-logic" ||
         tok == "set-option" ||
         tok == "define-fun" ||
         tok == "declare-fun" ||
         tok == "declare-const" ||
         tok == "not" ||
         tok == "and" ||
         tok == "or" ||
         tok == "synth-fun" ||
         tok == "constraint" ||
         tok == "check-synth" )
    {
      return token{token_kind::tok_keyword, tok};
    }
    else
    {
      return token{token_kind::tok_string, tok};
    }
  }

  bool run()
  {
    /* parse a sequence of S-expressions and comments */
    bool comment_mode = false;

    while ( !in.eof() )
    {
      /* comments */
      tok = get_token();

      if ( tok.value[0] == ';' )
      {
        comment_mode = true;
        continue;
      }

      if ( comment_mode )
      {
        if ( tok.kind == token_kind::tok_break )
        {
          comment_mode = false;
        }
      }
      else
      {
        /* parse an S-expression */
        auto id = sexpr();
        if ( id )
        {
          reader.on_top( *id );
          visit_sexpr( *id );
          reader.on_end( *id );
        }
      }
    }

    return true;
  }

  std::optional<index_type> sexpr()
  {
    if ( tok.value != "(" )
    {
      /* could not parse this S-expression */
      return std::nullopt;
    }

    std::vector<index_type> args;
    std::string keyword{};

    while ( !in.eof() )
    {
      tok = get_token();

      if ( tok.kind == token_kind::tok_keyword )
      {
        keyword = tok.value;
      }
      else if ( tok.value == ")" )
      {
        /* parse the end of this S-expression */
        break;
      }
      else if ( tok.value == "(" )
      {
        /* parse nested S-expression */
        auto const arg = sexpr();
        if ( !arg )
        {
          /* could not parse one of the nested S-expression */
          return std::nullopt;
        }
        args.emplace_back( *arg );
      }
      else
      {
        /* otherwise, create a leave node */
        args.emplace_back( reader.g.create_node( snode{tok.value} ) );
      }
    }

    /* create a node for the S-expression */
    if ( keyword == "" )
      return reader.g.create_node( snode{"()", args} );
    else
      return reader.g.create_node( snode{keyword, args} );
  }

  void visit_sexpr( index_type id )
  {
    auto const& node = reader.g[id];
    uint64_t const size = node.children.size();

    if ( size == 0u )
    {
      reader.on_leave( id );
    }
    else
    {
      reader.on_preorder( id );

      if ( size > 0u )
      {
        for ( const auto& child_id : node.children )
        {
          reader.on_child( child_id );
          visit_sexpr( child_id );
        }
      }

      reader.on_postorder( id );
    }
  }

protected:
  std::istream& in;
  Reader const& reader;

  token tok;
}; /* sexpr_parser */

template<typename Reader>
inline bool read_smt2( std::istream& in, Reader const& reader )
{
  sexpr_parser<Reader> parser( in, reader );
  return parser.run();
}

template<typename Reader>
inline bool read_smt2( std::string const& filename, Reader const& reader )
{
  bool result = false;
  std::ifstream in( filename, std::ifstream::in );
  if ( !in.is_open() )
  {
    return false;
  }
  result = read_smt2( in, reader );
  in.close();
  return result;
}

} /* lorina */
