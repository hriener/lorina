#include <catch.hpp>

#include <lorina/verilog.hpp>
#include <sstream>
#include <string>

using namespace lorina;

class simple_verilog_reader : public verilog_reader
{
public:
  struct module_info
  {
    std::string module_name;
    std::string inst_name;
    std::vector<std::string> params;
    std::vector<std::pair<std::string,std::string>> args;
  };

public:
  simple_verilog_reader() {}

  void on_module_header( const std::string& module_name, const std::vector<std::string>& inouts ) const override
  {
    (void)module_name;
    (void)inouts;
  }

  void on_inputs( const std::vector<std::string>& inputs, std::string const& size = "" ) const override
  {
    (void)size;
    _inputs = uint32_t( inputs.size() );
    _input_declarations.emplace_back( inputs, size );
  }

  void on_outputs( const std::vector<std::string>& outputs, std::string const& size = "" ) const override
  {
    (void)size;
    _outputs = uint32_t( outputs.size() );
    _output_declarations.emplace_back( outputs, size );
  }

  void on_wires( const std::vector<std::string>& wires, std::string const& size = "" ) const override
  {
    (void)size;
    _wires = uint32_t( wires.size() );
  }

  void on_assign( const std::string& lhs, const std::pair<std::string, bool>& rhs ) const override
  {
    (void)lhs;
    (void)rhs;
    ++_aliases;
  }

  void on_and( const std::string& lhs, const std::pair<std::string, bool>& op1, const std::pair<std::string, bool>& op2 ) const override
  {
    (void)lhs;
    (void)op1;
    (void)op2;
    ++_ands;
  }

  void on_nand( const std::string& lhs, const std::pair<std::string, bool>& op1, const std::pair<std::string, bool>& op2 ) const override
  {
    (void)lhs;
    (void)op1;
    (void)op2;
    ++_nands;
  }

  void on_or( const std::string& lhs, const std::pair<std::string, bool>& op1, const std::pair<std::string, bool>& op2 ) const override
  {
    (void)lhs;
    (void)op1;
    (void)op2;
    ++_ors;
  }

  void on_nor( const std::string& lhs, const std::pair<std::string, bool>& op1, const std::pair<std::string, bool>& op2 ) const override
  {
    (void)lhs;
    (void)op1;
    (void)op2;
    ++_nors;
  }

  void on_xor( const std::string& lhs, const std::pair<std::string, bool>& op1, const std::pair<std::string, bool>& op2 ) const override
  {
    (void)lhs;
    (void)op1;
    (void)op2;
    ++_xors;
  }

  void on_and3( const std::string& lhs, const std::pair<std::string, bool>& op1, const std::pair<std::string, bool>& op2, const std::pair<std::string, bool>& op3 ) const override
  {
    (void)lhs;
    (void)op1;
    (void)op2;
    (void)op3;
    ++_ands3;
  }

  void on_or3( const std::string& lhs, const std::pair<std::string, bool>& op1, const std::pair<std::string, bool>& op2, const std::pair<std::string, bool>& op3 ) const override
  {
    (void)lhs;
    (void)op1;
    (void)op2;
    (void)op3;
    ++_ors3;
  }

  void on_xor3( const std::string& lhs, const std::pair<std::string, bool>& op1, const std::pair<std::string, bool>& op2, const std::pair<std::string, bool>& op3 ) const override
  {
    (void)lhs;
    (void)op1;
    (void)op2;
    (void)op3;
    ++_xors3;
  }

  void on_maj3( const std::string& lhs, const std::pair<std::string, bool>& op1, const std::pair<std::string, bool>& op2, const std::pair<std::string, bool>& op3 ) const override
  {
    (void)lhs;
    (void)op1;
    (void)op2;
    (void)op3;
    ++_maj3;
  }

  void on_endmodule() const override {}

  void on_comment( const std::string& comment ) const override
  {
    (void)comment;
    ++_comments;
  }

  void on_parameter( const std::string& name, const std::string& value ) const override
  {
    _parameter_definitions.emplace_back( name, value );
    ++_parameter;
  }

  void on_module_instantiation( std::string const& module_name, std::vector<std::string> const& params, std::string const& inst_name,
                                std::vector<std::pair<std::string,std::string>> const& args ) const override
  {
    ++_instantiations;
    _instantiated_modules.emplace_back( module_info{module_name, inst_name, params, args} );
  }

  mutable uint32_t _inputs = 0;
  mutable uint32_t _outputs = 0;
  mutable uint32_t _wires = 0;
  mutable uint32_t _aliases = 0;
  mutable uint32_t _ands = 0;
  mutable uint32_t _nands = 0;
  mutable uint32_t _ors = 0;
  mutable uint32_t _nors = 0;
  mutable uint32_t _xors = 0;
  mutable uint32_t _ands3 = 0;
  mutable uint32_t _ors3 = 0;
  mutable uint32_t _xors3 = 0;
  mutable uint32_t _maj3 = 0;
  mutable uint32_t _comments = 0;
  mutable uint32_t _parameter = 0;
  mutable uint32_t _instantiations = 0;

  mutable std::vector<std::pair<std::string,std::string>> _parameter_definitions;
  mutable std::vector<std::pair<std::vector<std::string>,std::string>> _input_declarations;
  mutable std::vector<std::pair<std::vector<std::string>,std::string>> _output_declarations;
  mutable std::vector<module_info> _instantiated_modules;
}; /* simple_verilog_reader */

TEST_CASE( "Check return_code of read_verilog", "[verilog]")
{
  std::string broken_file =
    "module top( y1, a, b ) ;\n"
    "  input a , b ;\n"
    "  output y1 ;\n"
    "  wire g0 ;\n"
    "  g0 = a ^ b ;\n"
    "  assign y1 = g0 ;\n"
    "endmodule\n";

  {
    std::istringstream iss( broken_file );
    CHECK( read_verilog( iss, verilog_reader{} ) == return_code::parse_error );
  }

  std::string correct_file =
    "module top( y1, a, b ) ;\n"
    "  input a , b ;\n"
    "  output y1 ;\n"
    "  wire g0 ;\n"
    "  assign g0 = a ^ b ;\n"
    "  assign y1 = g0 ;\n"
    "endmodule\n";

  {
    std::istringstream iss( correct_file );
    CHECK( read_verilog( iss, verilog_reader{} ) == return_code::success );
  }
}

TEST_CASE( "Parse a simple Verilog file", "[verilog]" )
{
  std::string verilog_file =
    "module top( y1, y2, y3, y4, y5, a, b, c ) ;\n"
    "  input a , b , c ;\n"
    "  output y1 , y2, y3, y4, y5, y6, y7, y8 ;\n"
    "  wire zero, g0, g1 , g2 , g3 , g4, g5, g6, g7, g8 ;\n"
    "  assign zero = 0 ;\n"
    "  assign g0 = a ;\n"
    "  assign g1 = ~c ;\n"
    "  assign g2 = g0 & g1 ;\n"
    "  assign g3 = a | g2 ;\n"
    "  assign g4 = g2 ^ g3 ;\n"
    "  assign g5 = ( ~a & b ) | ( ~a & c ) | ( b & c ) ;\n"
    "  assign g6 = ~( a & b );\n"
    "  assign g7 = ~( a | b );\n"
    "  assign g8 = ~( a ^ b );\n"
    "  assign y1 = g4 ;\n"
    "  assign y2 = g5 ;\n"
    "  assign y3 = ~g0 & g1 & g2 ;\n"
    "  assign y4 = g1 | ~g2 | g3 ;\n"
    "  assign y5 = g3 ^ g4 ^ ~g5 ;\n"
    "  assign y6 = g6 ;\n"
    "  assign y7 = g7 ;\n"
    "  assign y8 = g8 ;\n"
    "endmodule\n";

  {
    std::istringstream iss( verilog_file );
    verilog_reader reader;
    auto result = read_verilog( iss, reader );
    CHECK( result == return_code::success );
  }

  {
    std::istringstream iss( verilog_file );

    simple_verilog_reader reader;
    auto result = read_verilog( iss, reader );
    CHECK( result == return_code::success );
    CHECK( reader._inputs == 3 );
    CHECK( reader._outputs == 8 );
    CHECK( reader._wires == 10 );
    CHECK( reader._aliases == 8 );
    CHECK( reader._ands == 1 );
    CHECK( reader._nands == 1 );
    CHECK( reader._ors == 1 );
    CHECK( reader._nors == 1 );
    CHECK( reader._xors == 1 );
    CHECK( reader._ands3 == 1 );
    CHECK( reader._ors3 == 1 );
    CHECK( reader._xors3 == 1 );
    CHECK( reader._maj3 == 1 );
  }
}

TEST_CASE( "Parse special characters in Verilog file", "[verilog]" )
{
  std::string verilog_file =
    "module top( \\y[0], \\y[1], \\x[0], \\x[1], \\x[2] ) ;\n"
    "  input \\x[0] , \\x[1] , \\x[2] ;\n"
    "  output \\y[0] , \\y[1] ;\n"
    "  wire zero, _g0, _g1 , _g2 , _g3 , _g4, _g5 ;\n"
    "  assign zero = 0 ;\n"
    "  assign _g0 = \\x[0] ;\n"
    "  assign _g1 = ~\\x[2] ;\n"
    "  assign _g2 = _g0 & _g1 ;\n"
    "  assign _g3 = \\x[0] | _g2 ;\n"
    "  assign _g4 = _g2 ^ _g3 ;\n"
    "  assign _g5 = ( ~\\x[0] & \\x[1] ) | ( ~\\x[0] & \\x[2] ) | ( \\x[1] & \\x[2] ) ;\n"
    "  assign \\y[0] = _g4 ;\n"
    "  assign \\y[1] = _g5 ;\n"
    "endmodule\n";

  std::istringstream iss( verilog_file );

  simple_verilog_reader reader;
  auto result = read_verilog( iss, reader );
  CHECK( result == return_code::success );
  CHECK( reader._inputs == 3 );
  CHECK( reader._outputs == 2 );
  CHECK( reader._wires == 7 );
  CHECK( reader._aliases == 5 );
  CHECK( reader._ands == 1 );
  CHECK( reader._ors == 1 );
  CHECK( reader._xors == 1 );
  CHECK( reader._maj3 == 1 );
}

TEST_CASE( "Parse comments in Verilog file", "[verilog]" )
{
  std::string verilog_file =
    "// comment at the beginning\n"
    "module top( \\y1, \\y2, \\a, \\b, \\c ) ;\n"
    "  input \\a , \\b , \\c ;\n"
    "  output \\y1 , \\y2 ;\n"
    "  wire zero, g0, g1 , g2 , g3 , g4, g5 ;\n"
    "  assign zero = 0 ; // the zero constant\n"
    "  assign g0 = \\a ;\n"
    "  assign g1 = ~\\c ;\n"
    "  assign g2 = g0 & g1 ;\n"
    "  // comment in the middle\n"
    "  assign g3 = \\a | g2 ;\n"
    "  assign g4 = g2 ^ g3 ;\n"
    "  assign g5 = ( ~\\a & \\b ) | ( ~\\a & \\c ) | ( \\b & \\c ) ;\n"
    "  assign \\y1 = g4 ;\n"
    "  assign \\y2 = g5 ;\n"
    "endmodule\n";

  std::istringstream iss( verilog_file );

  simple_verilog_reader reader;
  auto result = read_verilog( iss, reader );
  CHECK( result == return_code::success );
  CHECK( reader._inputs == 3 );
  CHECK( reader._outputs == 2 );
  CHECK( reader._wires == 7 );
  CHECK( reader._aliases == 5 );
  CHECK( reader._ands == 1 );
  CHECK( reader._ors == 1 );
  CHECK( reader._xors == 1 );
  CHECK( reader._maj3 == 1 );
  CHECK( reader._comments == 3 );
}

TEST_CASE( "Parse constants in Verilog file", "[verilog]" )
{
  std::string verilog_file =
    "module top( \\y1, \\y2 ) ;\n"
    "  output \\y1 , \\y2 ;\n"
    "  wire zero, g0 ;\n"
    "  assign zero = 0 ;\n"
    "  assign g0 = 1'b1 ;\n"
    "  assign \\y1 = 1'b0 ;\n"
    "  assign \\y2 = g0 ;\n"
    "endmodule\n";

  std::istringstream iss( verilog_file );

  simple_verilog_reader reader;
  auto result = read_verilog( iss, reader );
  CHECK( result == return_code::success );
  CHECK( reader._inputs == 0 );
  CHECK( reader._outputs == 2 );
  CHECK( reader._wires == 2 );
  CHECK( reader._aliases == 4 );
  CHECK( reader._ands == 0 );
  CHECK( reader._ors == 0 );
  CHECK( reader._xors == 0 );
  CHECK( reader._maj3 == 0 );
  CHECK( reader._comments == 0 );
}

TEST_CASE( "Parameter definition", "[verilog]" )
{
  std::string const verilog_file =
    "module top(a, b, c);\n"
    "  parameter A = 10;\n"
    "  parameter B = 20;\n"
    "  parameter C = 30;\n"
    "  input [A - 1:0] a;\n"
    "  input [B-1:0] b;\n"
    "  output [C -   1:0] c;\n"
    "endmodule";

  lorina::text_diagnostics consumer;
  lorina::diagnostic_engine diag( &consumer );
  std::istringstream iss( verilog_file );
  simple_verilog_reader reader;
  auto const result = read_verilog( iss, reader, &diag );
  CHECK( result == return_code::success );

  CHECK( reader._parameter_definitions.size() == 3u );
  CHECK( reader._parameter_definitions[0u].first == "A" );
  CHECK( reader._parameter_definitions[0u].second == "10" );
  CHECK( reader._parameter_definitions[1u].first == "B" );
  CHECK( reader._parameter_definitions[1u].second == "20" );
  CHECK( reader._parameter_definitions[2u].first == "C" );
  CHECK( reader._parameter_definitions[2u].second == "30" );

  CHECK( reader._input_declarations.size() == 2u );
  CHECK( reader._input_declarations[0u].first.size() == 1u );
  CHECK( reader._input_declarations[0u].first[0u] == "a" );
  CHECK( reader._input_declarations[0u].second == "A-1:0" );

  CHECK( reader._input_declarations[1u].first.size() == 1u );
  CHECK( reader._input_declarations[1u].first[0u] == "b" );
  CHECK( reader._input_declarations[1u].second == "B-1:0" );

  CHECK( reader._output_declarations.size() == 1u );
  CHECK( reader._output_declarations[0u].first.size() == 1u );
  CHECK( reader._output_declarations[0u].first[0u] == "c" );
  CHECK( reader._output_declarations[0u].second == "C-1:0" );
}

TEST_CASE( "Module instantiation with parameters", "[verilog]" )
{
  std::string const verilog_file =
    "module mod_mul( x1 , x2 , y1 );\n"
    "  input x1 , x2 ;\n"
    "  output y1 ;\n"
    "endmodule\n"
    "module mod_add( x1 , x2 , y1 );\n"
    "  input x1 , x2 ;\n"
    "  output y1 ;\n"
    "endmodule\n"
    "module mod_sub( x1 , x2 , y1 );\n"
    "  input x1 , x2 ;\n"
    "  output y1 ;\n"
    "endmodule\n"
    "module fp2mult(a0, a1, b0, b1, c0, c1);\n"
    "  parameter N = 5;\n"
    "  parameter M = 29;\n"
    "  input [N - 1:0] a0, a1, b0, b1;\n"
    "  output [N - 1:0] c0, c1;\n"
    "  wire [N - 1:0] w0, w1, w2, w3, w4, w5;\n"
    "  mod_mul #(M)    i1(.x1(a0), .x2(b0), .y1(w0));\n"
    "  mod_mul #(M)    i2(.x1(a1), .x2(b1), .y1(w1));\n"
    "  mod_add #(M)    i3(.x1(a0), .x2(a1), .y1(w2));\n"
    "  mod_add #(M,N)  i4(.x1(b0), .x2(b1), .y1(w3));\n"
    "  mod_sub #(M)    i5(.x1(w0), .x2(w1), .y1(c0));\n"
    "  mod_add #(M)    i6(.x1(w0), .x2(w1), .y1(w4));\n"
    "  mod_mul #(M, N) i7(.x1(w2), .x2(w3), .y1(w5));\n"
    "  mod_sub #(M)    i8(.x1(w4), .x2(w5), .y1(c1));\n"
    "endmodule";

  lorina::text_diagnostics consumer;
  lorina::diagnostic_engine diag( &consumer );

  std::istringstream iss( verilog_file );
  simple_verilog_reader reader;
  auto result = read_verilog( iss, reader, &diag );
  CHECK( result == return_code::success );
  CHECK( reader._inputs == 4 );
  CHECK( reader._outputs == 2 );
  CHECK( reader._wires == 6 );
  CHECK( reader._aliases == 0 );
  CHECK( reader._ands == 0 );
  CHECK( reader._ors == 0 );
  CHECK( reader._xors == 0 );
  CHECK( reader._maj3 == 0 );
  CHECK( reader._comments == 0 );
  CHECK( reader._parameter == 2 );
  CHECK( reader._instantiations == 8 );

  uint32_t cnt{0u};
  for ( auto i = 0u; i < reader._instantiated_modules.size(); ++i )
  {
    if ( reader._instantiated_modules[i].inst_name == "i1" )
    {
      CHECK( reader._instantiated_modules[i].module_name == "mod_mul" );
      CHECK( reader._instantiated_modules[i].args.size() == 3 );
      CHECK( reader._instantiated_modules[i].params.size() == 1 );
      CHECK( reader._instantiated_modules[i].params[0] == "M" );
      cnt++;
    }
    else if ( reader._instantiated_modules[i].inst_name == "i4" )
    {
      CHECK( reader._instantiated_modules[i].module_name == "mod_add" );
      CHECK( reader._instantiated_modules[i].args.size() == 3 );
      CHECK( reader._instantiated_modules[i].params.size() == 2 );
      CHECK( reader._instantiated_modules[i].params[0] == "M" );
      CHECK( reader._instantiated_modules[i].params[1] == "N" );
      cnt++;
    }
  }
  CHECK( cnt == 2 );
}

TEST_CASE( "Input and output registers", "[verilog]" )
{
  std::string verilog_file =
    "module gf24_inversion(x, y);\n"
    "    input [3:0] x;\n"
    "    output [3:0] y;\n"
    "    wire t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13;\n\n"
    "    assign t1 = x[2] ^ x[3];\n"
    "    assign t2 = x[2] & x[0];\n"
    "    assign t3 = x[1] ^ t2;\n"
    "    assign t4 = x[0] ^ x[1];\n"
    "    assign t5 = x[3] ^ t2;\n"
    "    assign t6 = t5 & t4;\n"
    "    assign t7 = t3 & t1;\n"
    "    assign t8 = x[0] & x[3];\n"
    "    assign t9 = t4 & t8;\n"
    "    assign t10 = t4 ^ t9;\n"
    "    assign t11 = x[1] & x[2];\n"
    "    assign t12 = t1 & t11;\n"
    "    assign t13 = t1 ^ t12;\n"
    "    assign y[0] = t2 ^ t13;\n"
    "    assign y[1] = x[3] ^ t7;\n"
    "    assign y[2] = t2 ^ t10;\n"
    "    assign y[3] = x[1] ^ t6;\n"
    "endmodule\n";

  lorina::text_diagnostics consumer;
  lorina::diagnostic_engine diag( &consumer );

  std::istringstream iss( verilog_file );
  simple_verilog_reader reader;
  auto result = read_verilog( iss, reader, &diag );
  CHECK( result == return_code::success );
  CHECK( reader._inputs == 1 );
  CHECK( reader._outputs == 1 );
  CHECK( reader._wires == 13 );
  CHECK( reader._aliases == 0 );
  CHECK( reader._ands == 7 );
  CHECK( reader._ors == 0 );
  CHECK( reader._xors == 10 );
  CHECK( reader._maj3 == 0 );
  CHECK( reader._comments == 0 );
  CHECK( reader._parameter == 0 );
  CHECK( reader._instantiations == 0 );
}

TEST_CASE( "Module instantiation without parameters and with output logic", "[verilog]" )
{
  std::string const verilog_file =
    "module buffer( i , o );\n"
    "  input i;\n"
    "  output o;\n"
    "endmodule\n"
    "module inverter( i , o );\n"
    "  input i;\n"
    "  output o;\n"
    "endmodule\n"
    "module top( x0 , x1 , y0 );\n"
    "  input x0 , x1 ;\n"
    "  output y0 ;\n"
    "  wire n3 , n4 , n5 , n6 ;\n"
    "  buffer buf_n4( .i (n3), .o (n4) );\n"
    "  buffer buf_n3( .i (x0), .o (n3) );\n"
    "  assign n5 = ~x1 & ~n4 ;\n"
    "  inverter inv_n6( .i (n5), .o (n6) );\n"
    "  assign y0 = n6 ;\n"
    "endmodule\n";

  lorina::text_diagnostics consumer;
  lorina::diagnostic_engine diag( &consumer );

  std::istringstream iss( verilog_file );
  simple_verilog_reader reader;
  auto result = read_verilog( iss, reader, &diag );
  CHECK( result == return_code::success );
  CHECK( reader._inputs == 2 );
  CHECK( reader._outputs == 1 );
  CHECK( reader._instantiations == 3 );
  CHECK( reader._instantiated_modules.size() == 3 );

  for ( auto i = 0u; i < reader._instantiated_modules.size(); ++i )
  {
    switch ( i )
    {
    case 0:
      CHECK( reader._instantiated_modules[i].module_name == "buffer" );
      CHECK( reader._instantiated_modules[i].inst_name == "buf_n3" );
      CHECK( reader._instantiated_modules[i].args[0].first == ".i" );
      CHECK( reader._instantiated_modules[i].args[0].second == "x0" );
      CHECK( reader._instantiated_modules[i].args[1].first == ".o" );
      CHECK( reader._instantiated_modules[i].args[1].second == "n3" );
      break;
    case 1:
      CHECK( reader._instantiated_modules[i].module_name == "buffer" );
      CHECK( reader._instantiated_modules[i].inst_name == "buf_n4" );
      CHECK( reader._instantiated_modules[i].args[0].first == ".i" );
      CHECK( reader._instantiated_modules[i].args[0].second == "n3" );
      CHECK( reader._instantiated_modules[i].args[1].first == ".o" );
      CHECK( reader._instantiated_modules[i].args[1].second == "n4" );
      break;
    case 2:
      CHECK( reader._instantiated_modules[i].module_name == "inverter" );
      CHECK( reader._instantiated_modules[i].inst_name == "inv_n6" );
      CHECK( reader._instantiated_modules[i].args[0].first == ".i" );
      CHECK( reader._instantiated_modules[i].args[0].second == "n5" );
      CHECK( reader._instantiated_modules[i].args[1].first == ".o" );
      CHECK( reader._instantiated_modules[i].args[1].second == "n6" );
      break;
    default:
      CHECK( false );
      break;
    }
  }
}
