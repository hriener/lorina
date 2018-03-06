#include <catch.hpp>

#include <lorina/bench.hpp>

using namespace lorina;

struct bench_statistics
{
  std::size_t number_of_inputs = 0;
  std::size_t number_of_outputs = 0;

  /* lines without input and outputs */
  std::size_t number_of_lines = 0;
};

class bench_statistics_reader : public bench_reader
{
public:
  bench_statistics_reader( bench_statistics& stats )
      : _stats( stats )
  {
  }

  virtual void on_input( const std::string& name ) const override
  {
    (void)name;
    ++_stats.number_of_inputs;
  }

  virtual void on_output( const std::string& name ) const override
  {
    (void)name;
    ++_stats.number_of_outputs;
  }

  virtual void on_gate( const std::vector<std::string>& inputs, const std::string& output, const std::string& type ) const override
  {
    (void)inputs;
    (void)output;
    (void)type;
    ++_stats.number_of_lines;
  }

  virtual void on_assign( const std::string& input, const std::string& output ) const override
  {
    (void)input;
    (void)output;
    ++_stats.number_of_lines;
  }

  bench_statistics& _stats;
}; /* bench_statistics_reader */

TEST_CASE( "bench_parse", "[bench]" )
{
  std::string bench_file =
      "INPUT(input_x[0])\n"
      "INPUT(input_x[1])\n"
      "OUTPUT(outport[0])\n"
      "OUTPUT(outport[1])\n"
      "n0 = vdd\n"
      "n1 = NOT(vdd)\n"
      "n2 = NOT(input_x[0])\n"
      "n3 = NOT(input_x[1])\n"
      "n4 = LUT 0x1 ( n1 )\n"
      "n5 = AND(n0, n4)\n"
      "n6 = OR(n2, n5)\n"
      "outport[0] = n6\n"
      "outport[1] = BUFF(outport[0])\n";

  std::istringstream iss( bench_file );

  bench_statistics stats;
  bench_statistics_reader reader( stats );
  auto result = read_bench( iss, reader );
  CHECK( result == return_code::success );
  CHECK( stats.number_of_inputs == 2 );
  CHECK( stats.number_of_outputs == 2 );
  CHECK( stats.number_of_lines == 9 );
}
