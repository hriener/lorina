#include <catch.hpp>

#include <lorina/genlib.hpp>
#include <sstream>
#include <string>

using namespace lorina;

struct gate
{
  std::string name;
  std::string expression;
  double area;
  std::vector<pin_spec> pins;
  std::string output_pin;
};

struct test_reader : public genlib_reader
{
public:
  explicit test_reader( std::vector<gate>& gates )
    : gates( gates )
  {}

  void on_gate( std::string const& name, std::string const& expression, double area, std::vector<pin_spec> const& pins, std::string const& output_pin ) const
  {
    gates.emplace_back( gate{name, expression, area, pins, output_pin} );
  }

public:
  std::vector<gate>& gates;
};

TEST_CASE( "instantiate genlib_reader", "[genlib]")
{
  std::string const genlib_file =
    "# comment\n"
    "GATE zero 0 O=0;\n"
    "GATE one 0	O=1;\n"
    "GATE inv1 1 O=!a; PIN * INV 1 999 1 1 1 1\n"
    "GATE buf 2 O=a; PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
    ;

  text_diagnostics consumer;
  diagnostic_engine diag( &consumer );
  std::istringstream iss( genlib_file );
  std::vector<gate> gate_definitions;
  CHECK( read_genlib( iss, genlib_reader{}, &diag ) == return_code::success );
}


TEST_CASE( "error cases", "[genlib]")
{
  {
    /* not all required fields have been specified */
    std::string const genlib_file = "GATE zero";
    std::istringstream iss( genlib_file );
    diagnostic_consumer consumer;
    diagnostic_engine diag( &consumer );
    CHECK( read_genlib( iss, genlib_reader{}, &diag ) == return_code::parse_error );
  }

  {
    /* the keyword `GATE` in the beginning is missing */
    std::string const genlib_file = "zero 0 O=0; PIN * INV 1 999 1 1 1 1";
    std::istringstream iss( genlib_file );
    diagnostic_consumer consumer;
    diagnostic_engine diag( &consumer );
    CHECK( read_genlib( iss, genlib_reader{}, &diag ) == return_code::parse_error );
  }

  {
    /* the expression is not terminated by a semicolon */
    std::string const genlib_file = "GATE zero 0 O=0 PIN * INV 1 999 1 1 1 1";
    std::istringstream iss( genlib_file );
    diagnostic_consumer consumer;
    diagnostic_engine diag( &consumer );
    CHECK( read_genlib( iss, genlib_reader{}, &diag ) == return_code::parse_error );
  }

  {
    /* the pin specification does not start with the keyword `PIN` */
    std::string const genlib_file = "GATE zero 0 O=0; a INV 1 999 1 1 1 1 1";
    std::istringstream iss( genlib_file );
    diagnostic_consumer consumer;
    diagnostic_engine diag( &consumer );
    CHECK( read_genlib( iss, genlib_reader{}, &diag ) == return_code::parse_error );
  }

  {
    /* the phase has been specified with an unknown keyword (NONINV is misspelled)  */
    std::string const genlib_file = "GATE zero 0 O=0; PIN * NOINV 1 999 1 1 1 1\n";
    std::istringstream iss( genlib_file );
    diagnostic_consumer consumer;
    diagnostic_engine diag( &consumer );
    CHECK( read_genlib( iss, genlib_reader{}, &diag ) == return_code::success );
  }

  {
    /* the PIN spec is incomplete and not all tokens are consumed  */
    std::string const genlib_file = "GATE zero 0 O=0; PIN a 1";
    std::istringstream iss( genlib_file );
    diagnostic_consumer consumer;
    diagnostic_engine diag( &consumer );
    CHECK( read_genlib( iss, genlib_reader{}, &diag ) == return_code::parse_error );
  }

  {
    /* multiple PINs of which one generic  */
    std::string const genlib_file = "GATE and 0 O=a*b; PIN a NONINV 1 999 1 1 1 1 PIN * NONINV 1 999 1 1 1 1";
    std::istringstream iss( genlib_file );
    diagnostic_consumer consumer;
    diagnostic_engine diag( &consumer );
    CHECK( read_genlib( iss, genlib_reader{}, &diag ) == return_code::parse_error );
  }

  {
    /* misspelled GATE with two gates definition  */
    std::string const genlib_file = "GTE and 0 O=a*b; PIN * NONINV 1 999 1 1 1 1\n"
                                    "GATE or 0 O=a+b; PIN * NONINV 1 999 1 1 1 1";
    std::istringstream iss( genlib_file );
    diagnostic_consumer consumer;
    diagnostic_engine diag( &consumer );
    CHECK( read_genlib( iss, genlib_reader{}, &diag ) == return_code::parse_error );
  }

  {
    /* empty genlib  */
    std::string const genlib_file = "";
    std::istringstream iss( genlib_file );
    diagnostic_consumer consumer;
    diagnostic_engine diag( &consumer );
    CHECK( read_genlib( iss, genlib_reader{}, &diag ) == return_code::success );
  }
}

TEST_CASE( "read GENLIB format", "[genlib]")
{
  std::string const genlib_file =
    "GATE zero 0 O=0;\n"
    "GATE one 0	O=1;\n"
    "GATE inv1 1 O=!a; PIN * INV 1 999 1 1 1 1\n"
    "GATE buf 2 Y=a; PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
    "GATE and2 2 Y=a * b; PIN * UNKNOWN 1.0 2.0 1.0 1.0 1.0 1.0;\n"
    ;

  text_diagnostics consumer;
  diagnostic_engine diag( &consumer );
  std::istringstream iss( genlib_file );
  std::vector<gate> gate_definitions;
  test_reader reader( gate_definitions );
  CHECK( read_genlib( iss, reader, &diag ) == return_code::success );

  CHECK( gate_definitions.size() == 5u );
  CHECK( gate_definitions[0u].name == "zero" );
  CHECK( gate_definitions[0u].expression == "0" );
  CHECK( gate_definitions[0u].area == 0.0 );
  CHECK( gate_definitions[0u].pins.empty() );
  CHECK( gate_definitions[0u].output_pin == "O" );

  CHECK( gate_definitions[1u].name == "one" );
  CHECK( gate_definitions[1u].expression == "1" );
  CHECK( gate_definitions[1u].area == 0.0 );
  CHECK( gate_definitions[1u].pins.empty() );
  CHECK( gate_definitions[1u].output_pin == "O" );

  CHECK( gate_definitions[2u].name == "inv1" );
  CHECK( gate_definitions[2u].expression == "!a" );
  CHECK( gate_definitions[2u].area == 1.0 );
  CHECK( gate_definitions[2u].pins.size() == 1u );
  CHECK( gate_definitions[2u].output_pin == "O" );

  CHECK( gate_definitions[3u].name == "buf" );
  CHECK( gate_definitions[3u].expression == "a" );
  CHECK( gate_definitions[3u].area == 2.0 );
  CHECK( gate_definitions[3u].pins.size() == 1u );
  CHECK( gate_definitions[3u].output_pin == "Y" );

  CHECK( gate_definitions[4u].name == "and2" );
  CHECK( gate_definitions[4u].expression == "a * b" );
  CHECK( gate_definitions[4u].area == 2.0 );
  CHECK( gate_definitions[4u].pins.size() == 1u );
  CHECK( gate_definitions[4u].output_pin == "Y" );
}

TEST_CASE( "PIN specification", "[genlib]")
{
  std::string const genlib_file =
    "GATE and2 1 O=a*b; PIN a INV 1.0 2.0 1.1 1.2 1.3 1.4 \n"
    "\tPIN b INV 1.0 2.0 1.0 1.0 1.0 1.0;\n"
    "#GATE zero 0 O=0;\n"
    "GATE and3 1 O=a*b*c; PIN * UNKNOWN 1.0 2.0 1.0 1.0 1.0 1.0;\n"
    ;

  text_diagnostics consumer;
  diagnostic_engine diag( &consumer );
  std::istringstream iss( genlib_file );
  std::vector<gate> gate_definitions;
  test_reader reader( gate_definitions );
  CHECK( read_genlib( iss, reader, &diag ) == return_code::success );

  CHECK( gate_definitions.size() == 2u );

  /* first gate */
  CHECK( gate_definitions[0u].pins.size() == 2u );
  CHECK( gate_definitions[0u].pins[0u].name == "a" );
  CHECK( gate_definitions[0u].pins[0u].phase == phase_type::INV );
  CHECK( gate_definitions[0u].pins[0u].input_load == 1.0 );
  CHECK( gate_definitions[0u].pins[0u].max_load == 2.0 );
  CHECK( gate_definitions[0u].pins[0u].rise_block_delay == 1.1 );
  CHECK( gate_definitions[0u].pins[0u].rise_fanout_delay == 1.2 );
  CHECK( gate_definitions[0u].pins[0u].fall_block_delay == 1.3 );
  CHECK( gate_definitions[0u].pins[0u].fall_fanout_delay == 1.4 );

  CHECK( gate_definitions[0u].pins[1u].name == "b" );
  CHECK( gate_definitions[0u].pins[1u].phase == phase_type::INV );
  CHECK( gate_definitions[0u].pins[1u].input_load == 1.0 );
  CHECK( gate_definitions[0u].pins[1u].max_load == 2.0 );
  CHECK( gate_definitions[0u].pins[1u].rise_block_delay == 1.0 );
  CHECK( gate_definitions[0u].pins[1u].rise_fanout_delay == 1.0 );
  CHECK( gate_definitions[0u].pins[1u].fall_block_delay == 1.0 );
  CHECK( gate_definitions[0u].pins[1u].fall_fanout_delay == 1.0 );

  /* second gate */
  CHECK( gate_definitions[1u].pins.size() == 1u );
  CHECK( gate_definitions[1u].pins[0u].name == "*" );
  CHECK( gate_definitions[1u].pins[0u].phase == phase_type::UNKNOWN );
  CHECK( gate_definitions[1u].pins[0u].input_load == 1.0 );
  CHECK( gate_definitions[1u].pins[0u].max_load == 2.0 );
  CHECK( gate_definitions[1u].pins[0u].rise_block_delay == 1.0 );
  CHECK( gate_definitions[1u].pins[0u].rise_fanout_delay == 1.0 );
  CHECK( gate_definitions[1u].pins[0u].fall_block_delay == 1.0 );
  CHECK( gate_definitions[1u].pins[0u].fall_fanout_delay == 1.0 );
}
