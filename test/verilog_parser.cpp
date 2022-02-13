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

TEST_CASE( "Expression", "[verilog]" )
{
  std::vector<std::string> sources;
  sources.emplace_back( "x" );
  sources.emplace_back( "(x)" );

  sources.emplace_back( "x & y" );
  sources.emplace_back( "(x) & y" );
  sources.emplace_back( "x & (y)" );
  sources.emplace_back( "(x & y)" );

  sources.emplace_back( "x | y" );
  sources.emplace_back( "(x) | y" );
  sources.emplace_back( "x | (y)" );
  sources.emplace_back( "(x | y)" );

  sources.emplace_back( "x ^ y" );
  sources.emplace_back( "(x) ^ y" );
  sources.emplace_back( "x ^ (y)" );
  sources.emplace_back( "(x ^ y)" );

  sources.emplace_back( "x & y & z" );
  sources.emplace_back( "x & y | z" );
  sources.emplace_back( "x & y ^ z" );
  sources.emplace_back( "x | y & z" );
  sources.emplace_back( "x | y | z" );
  sources.emplace_back( "x | y ^ z" );
  sources.emplace_back( "x ^ y & z" );
  sources.emplace_back( "x ^ y | z" );
  sources.emplace_back( "x ^ y ^ z" );

  sources.emplace_back( "x & (y | z)" );
  sources.emplace_back( "(x | y) & z" );

  sources.emplace_back( "~x" );
  sources.emplace_back( "~(x)" );
  sources.emplace_back( "(~x)" );
  sources.emplace_back( "~~x" );
  sources.emplace_back( "~a & b" );
  sources.emplace_back( "a & ~b" );
  sources.emplace_back( "~a & ~b" );
  sources.emplace_back( "~(a & b)" );
  sources.emplace_back( "(~a & b)" );
  sources.emplace_back( "(a & ~b)" );
  sources.emplace_back( "~(~a & b)" );
  sources.emplace_back( "~(a & ~b)" );
  sources.emplace_back( "~(~a & ~b)" );

  sources.emplace_back( "(x&~y)|~z)" );
  sources.emplace_back( "(foo^~bar)^baz)" );

  sources.emplace_back( "$maj{a,b,c}" );
  sources.emplace_back( "$maj{a,b,c,d,e}" );
  sources.emplace_back( "$maj{$maj{a,b,c},b,c}}" );
  sources.emplace_back( "$maj{a,$maj{a,b,c},c}}" );
  sources.emplace_back( "$maj{a,b,$maj{a,b,c}}" );
  sources.emplace_back( "$maj{~$maj{a,b,c},$maj{d,e,f},~$maj{g,h,i}}" );
  sources.emplace_back( "$maj{a&b,c|d,e^f}" );
  sources.emplace_back( "$maj{$and{a,b},$or{c,d},$xor{e,f}}" );

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
    verilog_ast_graph::ast_or_error ast = parser.consume_expression();
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

TEST_CASE( "Module instantiation", "[verilog]" )
{
  std::vector<std::string> sources;
  sources.emplace_back( "mod_mul #(2) mul0(.i0(a),.i1(b),.o0(c));\n" );
  sources.emplace_back( "mod_mul #(M)   mul0(.i0(a), .i1(b), .o0(c));\n" );
  sources.emplace_back( "mod_add #(M,N) add0(.i0(a), .i1(b), .o0(c));\n" );
  sources.emplace_back( "full_adder #(BITWIDTH) fa(.a(i[0]), .b(i[1]), .c(i[2]), .sum(o[0]), .carry(o[1]));\n" );

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
    verilog_ast_graph::ast_or_error ast = parser.consume_module_instantiation();
    CHECK( ast );
  }
}

TEST_CASE( "Parameter declaration", "[verilog]" )
{
  std::vector<std::string> sources;
  sources.emplace_back( "parameter WORD = 32;" );
  sources.emplace_back( "parameter SIZE = 10;" );
  sources.emplace_back( "parameter SIZE = N - 1;" );

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
    verilog_ast_graph::ast_or_error ast = parser.consume_parameter_declaration();
    CHECK( ast );
  }
}

TEST_CASE( "Assignment", "[verilog]" )
{
  std::vector<std::string> sources;
  sources.emplace_back( "assign maj = a & b | b & c | a & c;" );
  sources.emplace_back( "assign x[2] = x[0] ^ x[1];" );

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
    verilog_ast_graph::ast_or_error ast = parser.consume_assignment();
    CHECK( ast );
  }
}

TEST_CASE( "Module", "[verilog]" )
{
  std::vector<std::string> sources;
  sources.emplace_back(
      "module proto();\n"
      "endmodule\n"
    );
  sources.emplace_back(
      "module proto(x);\n"
      "endmodule\n"
    );
  sources.emplace_back(
      "module proto(x,y);\n"
      "endmodule\n"
    );
  sources.emplace_back(
      "module buffer(x,y);\n"
      "  input x;\n"
      "  output y;\n"
      "  assign y = x;\n"
      "endmodule\n"
    );
  sources.emplace_back(
      "module majority(x0,x1,x2,y);\n"
      "  input x0,x1,x2;\n"
      "  output y;\n"
      "  assign y = x0 & x1 | x1 & x2 | x0 & x2;\n"
      "endmodule\n"
    );
  sources.emplace_back(
      "module majority(x0,x1,x2,x3,x4,y);\n"
      "  input x0,x1,x2,x3,x4;\n"
      "  output y;\n"
      "  assign y = $maj{x0,x1,x2,x3,x4};\n"
      "endmodule\n"
    );
  sources.emplace_back(
      "module majority(x,y);\n"
      "  input [2:0] x;\n"
      "  output y;\n"
      "  assign y = x[0] & x[1] | x[1] & x[2] | x[0] & x[2];\n"
      "endmodule\n"
    );
  sources.emplace_back(
      "module full_adder(x,carry,sum);\n"
      "  input [2:0] x;\n"
      "  output carry, sum;\n"
      "  assign sum = x[0]^x[1]^x[2];\n"
      "  assign carry = $maj{x[0],x[1],x[2]};\n"
      "endmodule\n"
    );
  sources.emplace_back(
      "module parallel_and(x0,x1,y);\n"
      "  parameter N = 4;\n"
      "  input [N-1:0] x0;\n"
      "  input [N-1:0] x1;\n"
      "  output [N-1:0] y;\n"
      "  assign y = x0 & x1;\n"
      "endmodule\n"
    );
  sources.emplace_back(
      "module top( \\y[0] , \\y[1] , \\x[0] , \\x[1] , \\x[2] );\n"
      "  input \\x[0] , \\x[1] , \\x[2] ;\n"
      "  output \\y[0] , \\y[1] ;\n"
      "  wire zero, _g0, _g1, _g2, _g3, _g4, _g5;\n"
      "  assign zero = 1'b0;\n"
      "  assign _g0 = \\x[0] ;\n"
      "  assign _g1 = ~\\x[2] ;\n"
      "  assign _g2 = _g0 & _g1;\n"
      "  assign _g3 = \\x[0] | _g2;\n"
      "  assign _g4 = _g2 ^ _g3;\n"
      "  assign _g5 = ~\\x[0] & \\x[1] | ~\\x[0] & \\x[2] | \\x[1] & \\x[2] ;\n"
      "  assign \\y[0] = _g4;\n"
      "  assign \\y[1] = _g5;\n"
      "endmodule\n"
    );
  sources.emplace_back(
      "module fp2mult(a0, a1, b0, b1, c0, c1);\n"
      "  parameter N = 5;\n"
      "  parameter M = 29;\n"
      "  input [N - 1:0] a0, a1, b0, b1;\n"
      "  output [N - 1:0] c0, c1;\n"
      "  wire [N - 1:0] w0, w1, w2, w3, w4, w5;\n"
      "  mod_mul #(M)   i1(.x1(a0), .x2(b0), .y1(w0));\n"
      "  mod_mul #(M)   i2(.x1(a1), .x2(b1), .y1(w1));\n"
      "  mod_add #(M)   i3(.x1(a0), .x2(a1), .y1(w2));\n"
      "  mod_add #(M,N) i4(.x1(b0), .x2(b1), .y1(w3));\n"
      "  mod_sub #(M)   i5(.x1(w0), .x2(w1), .y1(c0));\n"
      "  mod_add #(M)   i6(.x1(w0), .x2(w1), .y1(w4));\n"
      "  mod_mul #(M,N) i7(.x1(w2), .x2(w3), .y1(w5));\n"
      "  mod_sub #(M)   i8(.x1(w4), .x2(w5), .y1(c1));\n"
      "endmodule"
    );

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
    verilog_ast_graph::ast_or_error ast = parser.consume_module();
    CHECK( ast );
  }
}
