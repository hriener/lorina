#include <catch.hpp>

#include <lorina/verilog/parser.hpp>
#include <lorina/verilog/ast_stringifier.hpp>
#include <sstream>
#include <fmt/format.h>

using namespace lorina;

TEST_CASE( "AST Visitor", "[verilog]" )
{
  verilog_ast_graph ag;
  ast_id n = ag.create_identifier( "test" );

  ast_stringifier vis( &ag );
  ag.node_ptr( n )->accept( vis );
  // vis.print();

  CHECK( vis.string_of( n ) == "test" );
}

TEST_CASE( "Visit identifier list", "[verilog]" )
{
  std::istringstream in( "a, b, c" );
  std::noskipws( in );

  using Lexer = verilog_lexer<std::istream_iterator<char>>;
  Lexer lexer( ( std::istream_iterator<char>(in) ), std::istream_iterator<char>());

  verilog_ast_graph ag;
  ast_stringifier vis( &ag );

  text_diagnostics consumer;
  diagnostic_engine diag( &consumer );

  verilog_parser parser( lexer, ag, &diag );
  verilog_ast_graph::ast_or_error identifier_list = parser.consume_identifier_list();
  assert( identifier_list );

  ag.node_ptr( identifier_list.ast() )->accept( vis );
  // vis.print();

  CHECK( vis.string_of( identifier_list.ast() ) == "a, b, c" );
}

TEST_CASE( "Visit expression", "[verilog]" )
{
  std::vector<std::pair<std::string, std::string>> sources;
  sources.emplace_back( std::make_pair( "x", "x" ) );
  sources.emplace_back( std::make_pair( "(x)", "x" ) );

  sources.emplace_back( std::make_pair( "x & y", "(x & y)" ) );
  sources.emplace_back( std::make_pair( "(x) & y", "(x & y)" ) );
  sources.emplace_back( std::make_pair( "x & (y)", "(x & y)" ) );
  sources.emplace_back( std::make_pair( "(x & y)", "(x & y)" ) );

  sources.emplace_back( std::make_pair( "x | y", "(x | y)" ) );
  sources.emplace_back( std::make_pair( "(x) | y", "(x | y)" ) );
  sources.emplace_back( std::make_pair( "x | (y)", "(x | y)" ) );
  sources.emplace_back( std::make_pair( "(x | y)", "(x | y)" ) );

  sources.emplace_back( std::make_pair( "x ^ y", "(x ^ y)" ) );
  sources.emplace_back( std::make_pair( "(x) ^ y", "(x ^ y)" ) );
  sources.emplace_back( std::make_pair( "x ^ (y)", "(x ^ y)" ) );
  sources.emplace_back( std::make_pair( "(x ^ y)", "(x ^ y)" ) );

  sources.emplace_back( std::make_pair( "x & y & z", "(x & (y & z))" ) );
  sources.emplace_back( std::make_pair( "x & y | z", "((x & y) | z)" ) );
  sources.emplace_back( std::make_pair( "x & y ^ z", "((x & y) ^ z)" ) );
  sources.emplace_back( std::make_pair( "x | y & z", "(x | (y & z))" ) );
  sources.emplace_back( std::make_pair( "x | y | z", "(x | (y | z))" ) );
  sources.emplace_back( std::make_pair( "x | y ^ z", "((x | y) ^ z)" ) );
  sources.emplace_back( std::make_pair( "x ^ y & z", "(x ^ (y & z))" ) );
  sources.emplace_back( std::make_pair( "x ^ y | z", "(x ^ (y | z))" ) );
  sources.emplace_back( std::make_pair( "x ^ y ^ z", "(x ^ (y ^ z))" ) );

  sources.emplace_back( std::make_pair( "x & (y | z)", "(x & (y | z))" ) );
  sources.emplace_back( std::make_pair( "(x | y) & z", "((x | y) & z)" ) );

  sources.emplace_back( std::make_pair( "~x", "(~x)" ) );
  sources.emplace_back( std::make_pair( "~(x)", "(~x)" ) );
  sources.emplace_back( std::make_pair( "(~x)", "(~x)" ) );
  sources.emplace_back( std::make_pair( "~~x", "(~(~x))" ) );
  sources.emplace_back( std::make_pair( "~a & b", "((~a) & b)" ) );
  sources.emplace_back( std::make_pair( "a & ~b", "(a & (~b))" ) );
  sources.emplace_back( std::make_pair( "~a & ~b", "((~a) & (~b))" ) );
  sources.emplace_back( std::make_pair( "~(a & b)", "(~(a & b))" ) );
  sources.emplace_back( std::make_pair( "(~a & b)", "((~a) & b)" ) );
  sources.emplace_back( std::make_pair( "(a & ~b)", "(a & (~b))" ) );
  sources.emplace_back( std::make_pair( "~(~a & b)", "(~((~a) & b))" ) );
  sources.emplace_back( std::make_pair( "~(a & ~b)", "(~(a & (~b)))" ) );
  sources.emplace_back( std::make_pair( "~(~a & ~b)", "(~((~a) & (~b)))" ) );

  sources.emplace_back( std::make_pair( "(x&~y)|~z)", "((x & (~y)) | (~z))" ) );
  sources.emplace_back( std::make_pair( "(foo^~bar)^baz)", "((foo ^ (~bar)) ^ baz)" ) );

  sources.emplace_back( std::make_pair( "$maj{a,b,c}", "$maj{a, b, c}" ) );
  sources.emplace_back( std::make_pair( "$maj{a,b,c,d,e}", "$maj{a, b, c, d, e}" ) );
  sources.emplace_back( std::make_pair( "$maj{$maj{a,b,c},b,c}", "$maj{$maj{a, b, c}, b, c}" ) );
  sources.emplace_back( std::make_pair( "$maj{a,$maj{a,b,c},c}", "$maj{a, $maj{a, b, c}, c}" ) );
  sources.emplace_back( std::make_pair( "$maj{a,b,$maj{a,b,c}}", "$maj{a, b, $maj{a, b, c}}" ) );
  sources.emplace_back( std::make_pair( "$maj{~$maj{a,b,c},$maj{d,e,f},~$maj{g,h,i}}",
                                        "$maj{(~$maj{a, b, c}), $maj{d, e, f}, (~$maj{g, h, i})}" ) );
  sources.emplace_back( std::make_pair( "$maj{a&b,c|d,e^f}", "$maj{(a & b), (c | d), (e ^ f)}" ) );
  sources.emplace_back( std::make_pair( "$maj{$and{a,b},$or{c,d},$xor{e,f}}",
                                        "$maj{$and{a, b}, $or{c, d}, $xor{e, f}}" ) );

  for ( const auto& source : sources )
  {
    std::istringstream in( source.first );
    std::noskipws( in );

    using Lexer = verilog_lexer<std::istream_iterator<char>>;
    Lexer lexer( ( std::istream_iterator<char>(in) ), std::istream_iterator<char>());

    verilog_ast_graph ag;
    ast_stringifier vis( &ag );

    text_diagnostics consumer;
    diagnostic_engine diag( &consumer );

    verilog_parser parser( lexer, ag, &diag );
    verilog_ast_graph::ast_or_error expr = parser.consume_expression();
    assert( expr );

    ag.node_ptr( expr.ast() )->accept( vis );
    CHECK( vis.string_of( expr.ast() ) == source.second );
  }
}

TEST_CASE( "Visit arithmetic expression", "[verilog]" )
{
  std::vector<std::pair<std::string, std::string>> sources;
  sources.emplace_back( std::make_pair( "x", "x" ) );
  sources.emplace_back( std::make_pair( "x[0]", "x[0]" ) );
  sources.emplace_back( std::make_pair( "y[1]", "y[1]" ) );
  sources.emplace_back( std::make_pair( "(x)", "x" ) );

  sources.emplace_back( std::make_pair( "x*y", "(x * y)" ) );
  sources.emplace_back( std::make_pair( "(-x)*y", "((-x) * y)" ) );
  sources.emplace_back( std::make_pair( "x*(-y)", "(x * (-y))" ) );
  sources.emplace_back( std::make_pair( "(-x)*(-y)", "((-x) * (-y))" ) );
  sources.emplace_back( std::make_pair( "x[0]*y[1]", "(x[0] * y[1])" ) );

  sources.emplace_back( std::make_pair( "x+y", "(x + y)" ) );
  sources.emplace_back( std::make_pair( "(-x)+y", "((-x) + y)" ) );
  sources.emplace_back( std::make_pair( "x+(-y)", "(x + (-y))" ) );
  sources.emplace_back( std::make_pair( "(-x)+(-y)", "((-x) + (-y))" ) );
  sources.emplace_back( std::make_pair( "x[0]+y[1]", "(x[0] + y[1])" ) );

  sources.emplace_back( std::make_pair( "2*x+1", "((2 * x) + 1)" ) );
  sources.emplace_back( std::make_pair( "x-1", "(x + (-1))" ) );

  for ( const auto& source : sources )
  {
    std::istringstream in( source.first );
    std::noskipws( in );

    using Lexer = verilog_lexer<std::istream_iterator<char>>;
    Lexer lexer( ( std::istream_iterator<char>(in) ), std::istream_iterator<char>());

    verilog_ast_graph ag;
    ast_stringifier vis( &ag );

    text_diagnostics consumer;
    diagnostic_engine diag( &consumer );

    verilog_parser parser( lexer, ag, &diag );
    verilog_ast_graph::ast_or_error expr = parser.consume_arithmetic_expression();
    assert( expr );

    ag.node_ptr( expr.ast() )->accept( vis );
    CHECK( vis.string_of( expr.ast() ) == source.second );
  }
}

TEST_CASE( "Visit input declaration", "[verilog]" )
{
  std::vector<std::pair<std::string, std::string>> sources;
  sources.emplace_back( std::make_pair( "input a;", "input a;" ) );
  sources.emplace_back( std::make_pair( "input a,b,c;", "input a, b, c;" ) );
  sources.emplace_back( std::make_pair( "input [7:0] a;", "input [7:0] a;" ) );
  sources.emplace_back( std::make_pair( "input [7:0] a, b, c;", "input [7:0] a, b, c;" ) );
  sources.emplace_back( std::make_pair( "input [N-1:0] a;", "input [(N + (-1)):0] a;" ) );

  for ( const auto& source : sources )
  {
    std::istringstream in( source.first );
    std::noskipws( in );

    using Lexer = verilog_lexer<std::istream_iterator<char>>;
    Lexer lexer( ( std::istream_iterator<char>(in) ), std::istream_iterator<char>());

    verilog_ast_graph ag;
    ast_stringifier vis( &ag );

    text_diagnostics consumer;
    diagnostic_engine diag( &consumer );

    verilog_parser parser( lexer, ag, &diag );
    verilog_ast_graph::ast_or_error expr = parser.consume_input_declaration();
    assert( expr );

    ag.node_ptr( expr.ast() )->accept( vis );
    CHECK( vis.string_of( expr.ast() ) == source.second );
  }
}

TEST_CASE( "Visit output declaration", "[verilog]" )
{
  std::vector<std::pair<std::string, std::string>> sources;
  sources.emplace_back( std::make_pair( "output a;", "output a;" ) );
  sources.emplace_back( std::make_pair( "output a, b, c;", "output a, b, c;" ) );
  sources.emplace_back( std::make_pair( "output [7:0] a;", "output [7:0] a;" ) );
  sources.emplace_back( std::make_pair( "output [7:0] a, b, c;", "output [7:0] a, b, c;" ) );
  sources.emplace_back( std::make_pair( "output [N-1:0] a;", "output [(N + (-1)):0] a;" ) );

  for ( const auto& source : sources )
  {
    std::istringstream in( source.first );
    std::noskipws( in );

    using Lexer = verilog_lexer<std::istream_iterator<char>>;
    Lexer lexer( ( std::istream_iterator<char>(in) ), std::istream_iterator<char>());

    verilog_ast_graph ag;
    ast_stringifier vis( &ag );

    text_diagnostics consumer;
    diagnostic_engine diag( &consumer );

    verilog_parser parser( lexer, ag, &diag );
    verilog_ast_graph::ast_or_error expr = parser.consume_output_declaration();
    assert( expr );

    ag.node_ptr( expr.ast() )->accept( vis );
    CHECK( vis.string_of( expr.ast() ) == source.second );
  }
}

TEST_CASE( "Visit wire declaration", "[verilog]" )
{
  std::vector<std::pair<std::string, std::string>> sources;
  sources.emplace_back( std::make_pair( "wire a;", "wire a;" ) );
  sources.emplace_back( std::make_pair( "wire a, b, c;", "wire a, b, c;" ) );
  sources.emplace_back( std::make_pair( "wire [7:0] a;", "wire [7:0] a;" ) );
  sources.emplace_back( std::make_pair( "wire [7:0] a, b, c;", "wire [7:0] a, b, c;" ) );
  sources.emplace_back( std::make_pair( "wire [N-1:0] a;", "wire [(N + (-1)):0] a;" ) );

  for ( const auto& source : sources )
  {
    std::istringstream in( source.first );
    std::noskipws( in );

    using Lexer = verilog_lexer<std::istream_iterator<char>>;
    Lexer lexer( ( std::istream_iterator<char>(in) ), std::istream_iterator<char>());

    verilog_ast_graph ag;
    ast_stringifier vis( &ag );

    text_diagnostics consumer;
    diagnostic_engine diag( &consumer );

    verilog_parser parser( lexer, ag, &diag );
    verilog_ast_graph::ast_or_error expr = parser.consume_wire_declaration();
    assert( expr );

    ag.node_ptr( expr.ast() )->accept( vis );
    CHECK( vis.string_of( expr.ast() ) == source.second );
  }
}

TEST_CASE( "Visit module instantiation", "[verilog]" )
{
  std::vector<std::pair<std::string, std::string>> sources;
  sources.emplace_back(
    std::make_pair( "mod_mul #(2) mul0(.i0(a),.i1(b),.o0(c));", "mod_mul #(2) mul0(.i0(a), .i1(b), .o0(c));" )
  );
  sources.emplace_back(
    std::make_pair( "mod_mul #(M)   mul0(.i0(a), .i1(b), .o0(c));", "mod_mul #(M) mul0(.i0(a), .i1(b), .o0(c));" )
  );
  sources.emplace_back(
    std::make_pair( "mod_add #(M,N) add0(.i0(a), .i1(b), .o0(c));", "mod_add #(M,N) add0(.i0(a), .i1(b), .o0(c));" )
  );
  sources.emplace_back(
    std::make_pair( "full_adder #(BITWIDTH) fa(.a(i[0]), .b(i[1]), .c(i[2]), .sum(o[0]), .carry(o[1]));",
                    "full_adder #(BITWIDTH) fa(.a(i[0]), .b(i[1]), .c(i[2]), .sum(o[0]), .carry(o[1]));" )
  );

  for ( const auto& source : sources )
  {
    std::istringstream in( source.first );
    std::noskipws( in );

    using Lexer = verilog_lexer<std::istream_iterator<char>>;
    Lexer lexer( ( std::istream_iterator<char>(in) ), std::istream_iterator<char>());

    verilog_ast_graph ag;
    ast_stringifier vis( &ag );

    text_diagnostics consumer;
    diagnostic_engine diag( &consumer );

    verilog_parser parser( lexer, ag, &diag );
    verilog_ast_graph::ast_or_error expr = parser.consume_module_instantiation();
    assert( expr );

    ag.node_ptr( expr.ast() )->accept( vis );
    CHECK( vis.string_of( expr.ast() ) == source.second );
  }
}

TEST_CASE( "Visit parameter declaration", "[verilog]" )
{
  std::vector<std::pair<std::string, std::string>> sources;
  sources.emplace_back( std::make_pair( "parameter WORD = 32;", "parameter WORD = 32;" ) );
  sources.emplace_back( std::make_pair( "parameter SIZE = 10;", "parameter SIZE = 10;" ) );
  sources.emplace_back( std::make_pair( "parameter SIZE = N - 1;", "parameter SIZE = (N + (-1));" ) );

  for ( const auto& source : sources )
  {
    std::istringstream in( source.first );
    std::noskipws( in );

    using Lexer = verilog_lexer<std::istream_iterator<char>>;
    Lexer lexer( ( std::istream_iterator<char>(in) ), std::istream_iterator<char>());

    verilog_ast_graph ag;
    ast_stringifier vis( &ag );

    text_diagnostics consumer;
    diagnostic_engine diag( &consumer );

    verilog_parser parser( lexer, ag, &diag );
    verilog_ast_graph::ast_or_error expr = parser.consume_parameter_declaration();
    assert( expr );

    ag.node_ptr( expr.ast() )->accept( vis );
    CHECK( vis.string_of( expr.ast() ) == source.second );
  }
}

TEST_CASE( "Visit assignment", "[verilog]" )
{
  std::vector<std::pair<std::string, std::string>> sources;
  sources.emplace_back( std::make_pair( "assign maj = a & b | b & c | a & c;", "assign maj = ((a & b) | ((b & c) | (a & c)));" ) );
  sources.emplace_back( std::make_pair( "assign x[2] = x[0] ^ x[1];", "assign x[2] = (x[0] ^ x[1]);" ) );

  for ( const auto& source : sources )
  {
    std::istringstream in( source.first );
    std::noskipws( in );

    using Lexer = verilog_lexer<std::istream_iterator<char>>;
    Lexer lexer( ( std::istream_iterator<char>(in) ), std::istream_iterator<char>());

    verilog_ast_graph ag;
    ast_stringifier vis( &ag );

    text_diagnostics consumer;
    diagnostic_engine diag( &consumer );

    verilog_parser parser( lexer, ag, &diag );
    verilog_ast_graph::ast_or_error expr = parser.consume_assignment();
    assert( expr );

    ag.node_ptr( expr.ast() )->accept( vis );
    CHECK( vis.string_of( expr.ast() ) == source.second );
  }
}

TEST_CASE( "Visit module", "[verilog]" )
{
  std::vector<std::pair<std::string, std::string>> sources;
  sources.emplace_back( std::make_pair(
      "module proto();\n"
      "endmodule\n",
      "module proto();\n"
      "endmodule\n"
    ) );
  sources.emplace_back( std::make_pair(
      "module proto(x);\n"
      "endmodule\n",
      "module proto(x);\n"
      "endmodule\n"
    ) );
  sources.emplace_back( std::make_pair(
      "module proto(x,y);\n"
      "endmodule\n",
      "module proto(x, y);\n"
      "endmodule\n"
    ) );
  sources.emplace_back( std::make_pair(
      "module buffer(x,y);\n"
      "  input x;\n"
      "  output y;\n"
      "  assign y = x;\n"
      "endmodule\n",
      "module buffer(x, y);\n"
      "input x;\n"
      "output y;\n"
      "assign y = x;\n"
      "endmodule\n"
    ) );
  sources.emplace_back( std::make_pair(
      "module majority(x0,x1,x2,y);\n"
      "  input x0,x1,x2;\n"
      "  output y;\n"
      "  assign y = x0 & x1 | x1 & x2 | x0 & x2;\n"
      "endmodule\n",
      "module majority(x0, x1, x2, y);\n"
      "input x0, x1, x2;\n"
      "output y;\n"
      "assign y = ((x0 & x1) | ((x1 & x2) | (x0 & x2)));\n"
      "endmodule\n"
    ) );
  sources.emplace_back( std::make_pair(
      "module majority(x0,x1,x2,x3,x4,y);\n"
      "  input x0,x1,x2,x3,x4;\n"
      "  output y;\n"
      "  assign y = $maj{x0,x1,x2,x3,x4};\n"
      "endmodule\n",
      "module majority(x0, x1, x2, x3, x4, y);\n"
      "input x0, x1, x2, x3, x4;\n"
      "output y;\n"
      "assign y = $maj{x0, x1, x2, x3, x4};\n"
      "endmodule\n"
    ) );
  sources.emplace_back( std::make_pair(
      "module majority(x,y);\n"
      "  input [2:0] x;\n"
      "  output y;\n"
      "  assign y = x[0] & x[1] | x[1] & x[2] | x[0] & x[2];\n"
      "endmodule\n",
      "module majority(x, y);\n"
      "input [2:0] x;\n"
      "output y;\n"
      "assign y = ((x[0] & x[1]) | ((x[1] & x[2]) | (x[0] & x[2])));\n"
      "endmodule\n"
    ) );
  sources.emplace_back( std::make_pair(
      "module full_adder(x,carry,sum);\n"
      "  input [2:0] x;\n"
      "  output carry, sum;\n"
      "  assign sum = x[0]^x[1]^x[2];\n"
      "  assign carry = $maj{x[0],x[1],x[2]};\n"
      "endmodule\n",
      "module full_adder(x, carry, sum);\n"
      "input [2:0] x;\n"
      "output carry, sum;\n"
      "assign sum = (x[0] ^ (x[1] ^ x[2]));\n"
      "assign carry = $maj{x[0], x[1], x[2]};\n"
      "endmodule\n"
    ) );

  for ( const auto& source : sources )
  {
    std::istringstream in( source.first );
    std::noskipws( in );

    using Lexer = verilog_lexer<std::istream_iterator<char>>;
    Lexer lexer( ( std::istream_iterator<char>(in) ), std::istream_iterator<char>());

    verilog_ast_graph ag;
    ast_stringifier vis( &ag );

    text_diagnostics consumer;
    diagnostic_engine diag( &consumer );

    verilog_parser parser( lexer, ag, &diag );
    verilog_ast_graph::ast_or_error expr = parser.consume_module();
    assert( expr );

    ag.node_ptr( expr.ast() )->accept( vis );
    CHECK( vis.string_of( expr.ast() ) == source.second );
  }
}
