#include <catch.hpp>

#include <lorina/verilog/lexer.hpp>
#include <sstream>
#include <fmt/format.h>

using namespace lorina;

TEST_CASE( "Lexical analysis", "[verilog]")
{
  std::string source{
    "/* TEST */\n"
    "module TEST(x,y,z);\n"
    "  input x, y; // input\n"
    "  output z; // output\n"
    "  assign z = x & y; // assign\n"
    "endmodule"};

  std::istringstream in( source );
  std::noskipws( in );

  using Lexer = verilog_lexer<std::istream_iterator<char>>;
  Lexer lexer( ( std::istream_iterator<char>(in) ), std::istream_iterator<char>());

  uint32_t token_id;
  uint32_t counter{0};
  while ( ( token_id = lexer.next_token() ) != Lexer::TID_EOF )
  {
    const std::string lexem = lexer.get( token_id ).lexem;
    // fmt::print( "token_id = {}: lexem = {}\n", token_id, lexem );
    switch (counter++)
    {
    case  0:
      {
        CHECK( token_id == Lexer::TID_KW_MODULE );
        CHECK( lexem == "module" );
      }
      break;
    case  1:
      {
        CHECK( token_id >= Lexer::TID_LAST );
        CHECK( lexem == "TEST" );
      }
      break;
    case  2:
      {
        CHECK( token_id == Lexer::TID_OP_LPAREN );
        CHECK( lexem == "(" );
      }
      break;
    case  3:
      {
        CHECK( token_id >= Lexer::TID_LAST );
        CHECK( lexem == "x" );
      }
      break;
    case  4:
      {
        CHECK( token_id == Lexer::TID_OP_COMMA );
        CHECK( lexem == "," );
      }
      break;
    case  5:
      {
        CHECK( token_id >= Lexer::TID_LAST );
        CHECK( lexem == "y" );
      }
      break;
    case  6:
      {
        CHECK( token_id == Lexer::TID_OP_COMMA );
        CHECK( lexem == "," );
      }
      break;
    case  7:
      {
        CHECK( token_id >= Lexer::TID_LAST );
        CHECK( lexem == "z" );
      }
      break;
    case  8:
      {
        CHECK( token_id == Lexer::TID_OP_RPAREN );
        CHECK( lexem == ")" );
      }
      break;
    case  9:
      {
        CHECK( token_id == Lexer::TID_OP_SEMICOLON );
        CHECK( lexem == ";" );
      }
      break;
    case 10:
      {
        CHECK( token_id == Lexer::TID_KW_INPUT );
        CHECK( lexem == "input" );
      }
      break;
    case 11:
      {
        CHECK( token_id >= Lexer::TID_LAST );
        CHECK( lexem == "x" );
      }
      break;
    case 12:
      {
        CHECK( token_id == Lexer::TID_OP_COMMA );
        CHECK( lexem == "," );
      }
      break;
    case 13:
      {
        CHECK( token_id >= Lexer::TID_LAST );
        CHECK( lexem == "y" );
      }
      break;
    case 14:
      {
        CHECK( token_id == Lexer::TID_OP_SEMICOLON );
        CHECK( lexem == ";" );
      }
      break;
    case 15:
      {
        CHECK( token_id == Lexer::TID_KW_OUTPUT );
        CHECK( lexem == "output" );
      }
      break;
    case 16:
      {
        CHECK( token_id >= Lexer::TID_LAST );
        CHECK( lexem == "z" );
      }
      break;
    case 17:
      {
        CHECK( token_id >= Lexer::TID_OP_SEMICOLON );
        CHECK( lexem == ";" );
      }
      break;
    case 18:
      {
        CHECK( token_id == Lexer::TID_KW_ASSIGN );
        CHECK( lexem == "assign" );
      }
      break;
    case 19:
      {
        CHECK( token_id >= Lexer::TID_LAST );
        CHECK( lexem == "z" );
      }
      break;
    case 20:
      {
        CHECK( token_id == Lexer::TID_OP_ASSIGN );
        CHECK( lexem == "=" );
      }
      break;
    case 21:
      {
        CHECK( token_id >= Lexer::TID_LAST );
        CHECK( lexem == "x" );
      }
      break;
    case 22:
      {
        CHECK( token_id == Lexer::TID_OP_BITWISE_AND );
        CHECK( lexem == "&" );
      }
      break;
    case 23:
      {
        CHECK( token_id >= Lexer::TID_LAST );
        CHECK( lexem == "y" );
      }
      break;
    case 24:
      {
        CHECK( token_id == Lexer::TID_OP_SEMICOLON );
        CHECK( lexem == ";" );
      }
      break;
    case 25:
      {
        CHECK( token_id == Lexer::TID_KW_ENDMODULE );
        CHECK( lexem == "endmodule" );
      }
      break;
    default:
      CHECK( false );
    }
  }
  CHECK( counter == 26 );
}
