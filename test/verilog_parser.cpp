#include <catch.hpp>

#include <lorina/verilog/parser.hpp>
#include <sstream>
#include <fmt/format.h>

using namespace lorina;

TEST_CASE( "Numeral", "[verilog]" )
{
  std::vector<std::string> sources;
  sources.emplace_back( "0" );
  sources.emplace_back( "1" );
  sources.emplace_back( "10" );
  sources.emplace_back( "12'o7770" );
  sources.emplace_back( "8'b110_01100" );
  sources.emplace_back( "16'h7fff" );

  for ( const auto& source : sources )
  {
    std::istringstream in( source );
    std::noskipws( in );

    using Lexer = verilog_lexer<std::istream_iterator<char>>;
    Lexer lexer( ( std::istream_iterator<char>(in) ), std::istream_iterator<char>());

    verilog_ast_graph ag;

    text_diagnostics consumer;
    diagnostic_engine diag( &consumer );

    verilog_parser parser( lexer, ag, &diag );
    verilog_ast_graph::ast_or_error ast = parser.consume_numeral();
    CHECK( ast );
  }
}

TEST_CASE( "Identifier", "[verilog]" )
{
  std::vector<std::string> sources;
  sources.emplace_back( "a" );
  sources.emplace_back( "b" );
  sources.emplace_back( "c" );
  sources.emplace_back( "test123" );
  sources.emplace_back( "_test42" );
  sources.emplace_back( "\test-" );

  for ( const auto& source : sources )
  {
    std::istringstream in( source );
    std::noskipws( in );

    using Lexer = verilog_lexer<std::istream_iterator<char>>;
    Lexer lexer( ( std::istream_iterator<char>(in) ), std::istream_iterator<char>());

    verilog_ast_graph ag;

    text_diagnostics consumer;
    diagnostic_engine diag( &consumer );

    verilog_parser parser( lexer, ag, &diag );
    verilog_ast_graph::ast_or_error ast = parser.consume_identifier();
    CHECK( ast );
  }
}

TEST_CASE( "List of identifiers", "[verilog]" )
{
  std::vector<std::string> sources;
  sources.emplace_back( "a" );
  sources.emplace_back( "a, b, c" );

  for ( const auto& source : sources )
  {
    std::istringstream in( source );
    std::noskipws( in );

    using Lexer = verilog_lexer<std::istream_iterator<char>>;
    Lexer lexer( ( std::istream_iterator<char>(in) ), std::istream_iterator<char>());

    verilog_ast_graph ag;

    text_diagnostics consumer;
    diagnostic_engine diag( &consumer );

    verilog_parser parser( lexer, ag, &diag );
    verilog_ast_graph::ast_or_error ast = parser.consume_identifier_list();
    CHECK( ast );
  }
}

TEST_CASE( "Signal reference", "[verilog]" )
{
  std::vector<std::string> sources;
  sources.emplace_back( "x" );
  sources.emplace_back( "x[1]" );

  for ( const auto& source : sources )
  {
    std::istringstream in( source );
    std::noskipws( in );

    using Lexer = verilog_lexer<std::istream_iterator<char>>;
    Lexer lexer( ( std::istream_iterator<char>(in) ), std::istream_iterator<char>());

    verilog_ast_graph ag;

    text_diagnostics consumer;
    diagnostic_engine diag( &consumer );

    verilog_parser parser( lexer, ag, &diag );
    verilog_ast_graph::ast_or_error ast = parser.consume_signal_reference();
    CHECK( ast );
  }
}

TEST_CASE( "Arithmetic expression", "[verilog]" )
{
  std::vector<std::string> sources;
  sources.emplace_back( "x" );
  sources.emplace_back( "x[0]" );
  sources.emplace_back( "(x)" );

  sources.emplace_back( "x*y" );
  sources.emplace_back( "(-x)*y" );
  sources.emplace_back( "x*(-y)" );
  sources.emplace_back( "(-x)*(-y)" );
  sources.emplace_back( "x[0]*y[1]" );

  sources.emplace_back( "x+y" );
  sources.emplace_back( "(-x)+y" );
  sources.emplace_back( "x+(-y)" );
  sources.emplace_back( "(-x)+(-y)" );
  sources.emplace_back( "x[0]+y[1]" );

  sources.emplace_back( "2*x+1" );
  sources.emplace_back( "x-1" );

  for ( const auto& source : sources )
  {
    std::istringstream in( source );
    std::noskipws( in );

    using Lexer = verilog_lexer<std::istream_iterator<char>>;
    Lexer lexer( ( std::istream_iterator<char>(in) ), std::istream_iterator<char>());

    verilog_ast_graph ag;

    text_diagnostics consumer;
    diagnostic_engine diag( &consumer );

    verilog_parser parser( lexer, ag, &diag );
    verilog_ast_graph::ast_or_error ast = parser.consume_arithmetic_expression();
    CHECK( ast );
  }
}

TEST_CASE( "Input declaration", "[verilog]" )
{
  std::vector<std::string> sources;
  sources.emplace_back( "input a;" );
  sources.emplace_back( "input a, b, c;" );
  sources.emplace_back( "input [7:0] a;" );
  sources.emplace_back( "input [7:0] a, b, c;" );
  sources.emplace_back( "input [N-1:0] a;" );

  for ( const auto& source : sources )
  {
    std::istringstream in( source );
    std::noskipws( in );

    using Lexer = verilog_lexer<std::istream_iterator<char>>;
    Lexer lexer( ( std::istream_iterator<char>(in) ), std::istream_iterator<char>());

    verilog_ast_graph ag;

    text_diagnostics consumer;
    diagnostic_engine diag( &consumer );

    verilog_parser parser( lexer, ag, &diag );
    verilog_ast_graph::ast_or_error ast = parser.consume_input_declaration();
    CHECK( ast );
  }
}

TEST_CASE( "Output declaration", "[verilog]" )
{
  std::vector<std::string> sources;
  sources.emplace_back( "output a;" );
  sources.emplace_back( "output a, b, c;" );
  sources.emplace_back( "output [7:0] a;" );
  sources.emplace_back( "output [7:0] a, b, c;" );
  sources.emplace_back( "output [N-1:0] a;" );

  for ( const auto& source : sources )
  {
    std::istringstream in( source );
    std::noskipws( in );

    using Lexer = verilog_lexer<std::istream_iterator<char>>;
    Lexer lexer( ( std::istream_iterator<char>(in) ), std::istream_iterator<char>());

    verilog_ast_graph ag;

    text_diagnostics consumer;
    diagnostic_engine diag( &consumer );

    verilog_parser parser( lexer, ag, &diag );
    verilog_ast_graph::ast_or_error ast = parser.consume_output_declaration();
    CHECK( ast );
  }
}

TEST_CASE( "Wire declaration", "[verilog]" )
{
  std::vector<std::string> sources;
  sources.emplace_back( "wire a;" );
  sources.emplace_back( "wire a, b, c;" );
  sources.emplace_back( "wire [7:0] a;" );
  sources.emplace_back( "wire [7:0] a, b, c;" );
  sources.emplace_back( "wire [N-1:0] a;" );

  for ( const auto& source : sources )
  {
    std::istringstream in( source );
    std::noskipws( in );

    using Lexer = verilog_lexer<std::istream_iterator<char>>;
    Lexer lexer( ( std::istream_iterator<char>(in) ), std::istream_iterator<char>());

    verilog_ast_graph ag;

    text_diagnostics consumer;
    diagnostic_engine diag( &consumer );

    verilog_parser parser( lexer, ag, &diag );
    verilog_ast_graph::ast_or_error ast = parser.consume_wire_declaration();
    CHECK( ast );
  }
}
