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
  \file parser.hpp
  \brief Verilog Parser

  \author Heinz Riener
*/

#pragma once

#include "../diagnostics.hpp"
#include "ast_graph.hpp"
#include "lexer.hpp"

namespace lorina
{

class verilog_parser
{
public:
  using Lexer = verilog_lexer<std::istream_iterator<char>>;

public:
  explicit verilog_parser( Lexer& lexer, verilog_ast_graph& ag, diagnostic_engine *diag )
    : lexer_( lexer )
    , ag_( ag )
    , diag_( diag )
  {
    token_id_ = lexer_.next_token();
  }

public:
  /* \brief Test if current token is a numeral
   *
   * \return Returns true iff current token is a numeral
   */
  inline bool is_numeral() const
  {
    const token s = lexer_.get( token_id_ );
    return s.kind == token_kind::TK_NUMERAL;
  }

  /* \brief Consumes a numeral
   *
   * \return Returns AST of the numeral or an error
   */
  inline verilog_ast_graph::ast_or_error consume_numeral()
  {
    assert( is_numeral() );

    const std::string& numeral = lexer_.get( token_id_ ).lexem;
    token_id_ = lexer_.next_token();
    return verilog_ast_graph::ast_or_error( ag_.create_numeral( numeral ) );
  }

  /* \brief Test if current token is the start of a range expression
   *
   * \return Returns true iff current token is the start of a range
   *         expression
   */
  inline bool is_range_expression() const
  {
    return token_id_ == Lexer::TID_OP_LSQUARED;
  }

  /* \brief Consume a range expression
   *
   * A range expression is a pair of a most-significant bit and a
   * least-significant bit.
   *
   * Production rule:
   *   RANGE_EXPR ::= `[` NUMERAL `:` NUMERAL `]`
   *
   * Example:
   *   [7:4]
   */
  inline verilog_ast_graph::ast_or_error consume_range_expression()
  {
    verilog_ast_graph::ast_or_error hi, lo;
    assert( token_id_ == Lexer::TID_OP_LSQUARED );
    token_id_ = lexer_.next_token(); // `[`

    hi = consume_numeral(); // consume_arithmetic_expression()
    if ( !hi )
      goto error;

    if ( token_id_ != Lexer::TID_OP_COLON )
      goto error;
    token_id_ = lexer_.next_token(); // `:`

    lo = consume_numeral(); // consume_arithmetic_expression()
    if ( !lo )
      goto error;

    if ( token_id_ != Lexer::TID_OP_RSQUARED )
      goto error;
    token_id_ = lexer_.next_token(); // `]`

    return verilog_ast_graph::ast_or_error( ag_.create_range_expression( hi.ast(), lo.ast() ) );

  error:
    fmt::print("[e] could not parse a range expression.\n");
    return verilog_ast_graph::ast_or_error();
  }

  /* \brief Test if current token is an identifier
   *
   * \return Returns true iff the current token is an identifier
   */
  inline bool is_identifier() const
  {
    const token s = lexer_.get( token_id_ );
    return s.kind == token_kind::TK_IDENTIFIER;
  }

  /* \brief Consume an identifier
   *
   * \return Returns AST of the identifier or an error
   */
  inline verilog_ast_graph::ast_or_error consume_identifier()
  {
    assert( is_identifier() );

    const std::string& identifier = lexer_.get( token_id_ ).lexem;
    token_id_ = lexer_.next_token();
    return verilog_ast_graph::ast_or_error( ag_.create_identifier( identifier ) );
  }

  /* \brief Consume a list of identifiers
   *
   * A list of identifiers separated by commas
   *
   * Production rule:
   *   IDENTIFIER_LIST ::= IDENTIFIER `,` ... `,` IDENTIFIER
   *
   * Example:
   *   a, b, c
   */
  inline verilog_ast_graph::ast_or_error consume_identifier_list()
  {
    verilog_ast_graph::ast_or_error identifier;
    std::vector<ast_id> identifiers;

    if ( !is_identifier() )
      goto error;

    /* first identifier */
    identifier = consume_identifier();
    if ( !identifier )
      goto error;
    identifiers.emplace_back( identifier.ast() );

    /* other identifiers */
    while ( token_id_ == Lexer::TID_OP_COMMA )
    {
      token_id_ = lexer_.next_token(); // `,`
      if ( !is_identifier() )
        goto error;

      identifier = consume_identifier();
      if ( !identifier )
        goto error;
      identifiers.emplace_back( identifier.ast() );
    }
    return verilog_ast_graph::ast_or_error( identifiers.size() == 1 ?
                                            identifiers[0] : ag_.create_identifier_list( identifiers ) );

  error:
    fmt::print("[e] could not parse an identifier list.\n");
    return verilog_ast_graph::ast_or_error();
  }

  /* \brief Consume input declaration
   *
   * An input declaration declares a list of identifiers either as
   * bit-level or word-level inputs.
   *
   * Production rule:
   *   INPUT_DECLARATIONS ::= `input` ( `[` MSB `:` LSB `]` )? IDENTIFIER `,` ... `,` IDENTIFIER `;`
   */
  inline verilog_ast_graph::ast_or_error consume_input_declaration()
  {
    verilog_ast_graph::ast_or_error identifiers, range;
    bool word_level = false;

    if ( token_id_ != Lexer::TID_KW_INPUT )
      goto error;
    token_id_ = lexer_.next_token(); // `input`

    if ( is_range_expression() )
    {
      word_level = true;
      range = consume_range_expression();
      if ( !range )
        goto error;
    }

    identifiers = consume_identifier_list();
    if ( !identifiers )
      goto error;

    if ( token_id_ != Lexer::TID_OP_SEMICOLON )
      goto error;
    token_id_ = lexer_.next_token(); // `;`

    if ( word_level )
    {
      return verilog_ast_graph::ast_or_error( ag_.create_input_declaration( identifiers.ast(), range.ast() ) );
    }
    else
    {
      return verilog_ast_graph::ast_or_error( ag_.create_input_declaration( identifiers.ast() ) );
    }

  error:
    fmt::print("[e] could not parse input declaration.\n");
    return verilog_ast_graph::ast_or_error();
  }

  /* \brief Consume output declaration
   *
   * An output declaration declares a list of identifiers either as
   * bit-level or word-level outputs.
   *
   * Production rule:
   *   OUTPUT_DECLARATIONS ::= `output` ( `[` MSB `:` LSB `]` )? IDENTIFIER `,` ... `,` IDENTIFIER `;`
   */
  inline verilog_ast_graph::ast_or_error consume_output_declaration()
  {
    verilog_ast_graph::ast_or_error identifiers, range;
    bool word_level = false;

    if ( token_id_ != Lexer::TID_KW_OUTPUT )
      goto error;
    token_id_ = lexer_.next_token(); // `output`

    if ( is_range_expression() )
    {
      word_level = true;
      range = consume_range_expression();
      if ( !range )
        goto error;
    }

    identifiers = consume_identifier_list();
    if ( !identifiers )
      goto error;

    if ( token_id_ != Lexer::TID_OP_SEMICOLON )
      goto error;
    token_id_ = lexer_.next_token(); // `;`

    if ( word_level )
    {
      return verilog_ast_graph::ast_or_error( ag_.create_output_declaration( identifiers.ast(), range.ast() ) );
    }
    else
    {
      return verilog_ast_graph::ast_or_error( ag_.create_output_declaration( identifiers.ast() ) );
    }

  error:
    fmt::print("[e] could not parse output declaration.\n");
    return verilog_ast_graph::ast_or_error();
  }

  /* \brief Consume wire declaration
   *
   * An wire declaration declares a list of identifiers either as
   * bit-level or word-level wires.
   *
   * Production rule:
   *   WIRE_DECLARATIONS ::= `wire` ( `[` MSB `:` LSB `]` )? IDENTIFIER `,` ... `,` IDENTIFIER `;`
   */
  inline verilog_ast_graph::ast_or_error consume_wire_declaration()
  {
    verilog_ast_graph::ast_or_error identifiers, range;
    bool word_level = false;

    if ( token_id_ != Lexer::TID_KW_WIRE )
      goto error;
    token_id_ = lexer_.next_token(); // `wire`

    if ( is_range_expression() )
    {
      word_level = true;
      range = consume_range_expression();
      if ( !range )
        goto error;
    }

    identifiers = consume_identifier_list();
    if ( !identifiers )
      goto error;

    if ( token_id_ != Lexer::TID_OP_SEMICOLON )
      goto error;
    token_id_ = lexer_.next_token(); // `;`

    if ( word_level )
    {
      return verilog_ast_graph::ast_or_error( ag_.create_wire_declaration( identifiers.ast(), range.ast() ) );
    }
    else
    {
      return verilog_ast_graph::ast_or_error( ag_.create_wire_declaration( identifiers.ast() ) );
    }

  error:
    fmt::print("[e] could not parse wire declaration.\n");
    return verilog_ast_graph::ast_or_error();
  }

protected:
  Lexer& lexer_;
  verilog_ast_graph& ag_;
  diagnostic_engine *diag_;
  uint32_t token_id_;
};

} // namespace lorina
