#include <catch.hpp>

#include <lorina/genlib.hpp>

using namespace lorina;

struct gate
{
  std::string name;
  std::string expression;
  double area;
  std::vector<pin_spec> pins;
};

struct test_reader : public genlib_reader
{
public:
  explicit test_reader( std::vector<gate>& gates )
    : gates( gates )
  {}

  void on_gate( std::string const& name, std::string const& expression, double area, std::vector<pin_spec> const& pins ) const
  {
    gates.emplace_back( gate{name, expression, area, pins} );
  }

public:
  std::vector<gate>& gates;
};

TEST_CASE( "GENLIB", "[genlib]")
{
  std::string const genlib_file =
    "GATE zero 0 O=0;\n"
    "GATE one 0	O=1;\n"
    "GATE inv1 1 O=!a; PIN * INV 1 999 1 1 1 1\n"
    "GATE buf 2 O=a; PIN * NONINV 1 999 1.0 1.0 1.0 1.0\n"
    ;

  diagnostic_engine diag;
  std::istringstream iss( genlib_file );
  std::vector<gate> gate_definitions;
  test_reader reader( gate_definitions );
  CHECK( read_genlib( iss, reader, &diag ) == return_code::success );

  CHECK( gate_definitions.size() == 4u );
  CHECK( gate_definitions[0u].name == "zero" );
  CHECK( gate_definitions[0u].expression == "0" );
  CHECK( gate_definitions[0u].area == 0.0 );
  CHECK( gate_definitions[0u].pins.empty() );

  CHECK( gate_definitions[1u].name == "one" );
  CHECK( gate_definitions[1u].expression == "1" );
  CHECK( gate_definitions[1u].area == 0.0 );
  CHECK( gate_definitions[1u].pins.empty() );

  CHECK( gate_definitions[2u].name == "inv1" );
  CHECK( gate_definitions[2u].expression == "!a" );
  CHECK( gate_definitions[2u].area == 1.0 );
  CHECK( gate_definitions[2u].pins.size() == 1u );

  CHECK( gate_definitions[3u].name == "buf" );
  CHECK( gate_definitions[3u].expression == "a" );
  CHECK( gate_definitions[3u].area == 2.0 );
  CHECK( gate_definitions[3u].pins.size() == 1u );
}

TEST_CASE( "PIN specification", "[genlib]")
{
  std::string const genlib_file =
    "GATE and2 1 O=a*b; PIN a INV 1.0 2.0 1.1 1.2 1.3 1.4 PIN b INV 1.0 2.0 1.0 1.0 1.0 1.0;\n"
    "GATE and3 1 O=a*b*c; PIN * UNKNOWN 1.0 2.0 1.0 1.0 1.0 1.0;\n"
    ;

  diagnostic_engine diag;
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
