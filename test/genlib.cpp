#include <catch.hpp>

#include <lorina/genlib.hpp>

using namespace lorina;

struct gate
{
  std::string name;
  std::string expression;
  double area;
  double delay;
};

struct test_reader : public genlib_reader
{
public:
  explicit test_reader( std::vector<gate>& gates )
    : gates( gates )
  {}

  void on_gate( std::string const& name, std::string const& expression, double area, std::optional<double> delay ) const
  {
    gates.emplace_back( gate{name, expression, area, delay ? *delay : 0.0} );
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
  CHECK( gate_definitions[0u].delay == 0.0 );

  CHECK( gate_definitions[1u].name == "one" );
  CHECK( gate_definitions[1u].expression == "1" );
  CHECK( gate_definitions[1u].area == 0.0 );
  CHECK( gate_definitions[1u].delay == 0.0 );

  CHECK( gate_definitions[2u].name == "inv1" );
  CHECK( gate_definitions[2u].expression == "!a" );
  CHECK( gate_definitions[2u].area == 1.0 );
  CHECK( gate_definitions[2u].delay == 1.0 );

  CHECK( gate_definitions[3u].name == "buf" );
  CHECK( gate_definitions[3u].expression == "a" );
  CHECK( gate_definitions[3u].area == 2.0 );
  CHECK( gate_definitions[3u].delay == 1.0 );
}
