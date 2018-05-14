#include <catch.hpp>

#include <lorina/aiger.hpp>
#include <fmt/format.h>
#include <fmt/printf.h>
#include <map>
#include <iostream>

using namespace lorina;

struct aiger_statistics
{
  /* header information AIGER v1.9 */
  std::size_t maximum_variable_index = 0;
  std::size_t number_of_inputs = 0;
  std::size_t number_of_latches = 0;
  std::size_t number_of_outputs = 0;
  std::size_t number_of_ands = 0;
  std::size_t number_of_bad_states = 0;
  std::size_t number_of_constraints = 0;
  std::size_t number_of_justice = 0;
  std::size_t number_of_fairness = 0;

  std::vector<std::tuple<unsigned, unsigned>> outputs;
  std::vector<std::tuple<unsigned,unsigned,aiger_reader::latch_init_value>> latches;
  std::vector<std::tuple<unsigned,unsigned,unsigned>> ands;
  std::vector<std::tuple<unsigned, unsigned>> bad_states;
  std::vector<std::tuple<unsigned, unsigned>> constraints;
  std::vector<std::tuple<unsigned, std::vector<unsigned>>> justice;
  std::vector<std::tuple<unsigned, unsigned>> fairness;

  std::map<unsigned, std::string> input_names;
  std::map<unsigned, std::string> output_names;
  std::string comment;
};

class aiger_statistics_reader : public aiger_reader
{
public:
  aiger_statistics_reader( aiger_statistics& stats )
      : _stats( stats )
  {
  }

  void on_header( std::size_t m, std::size_t i, std::size_t l, std::size_t o, std::size_t a,
                  std::size_t b, std::size_t c, std::size_t j, std::size_t f ) const override
  {
    _stats.maximum_variable_index = m;
    _stats.number_of_inputs = i;
    _stats.number_of_latches = l;
    _stats.number_of_outputs = o;
    _stats.number_of_ands = a;
    _stats.number_of_bad_states = b;
    _stats.number_of_constraints = c;
    _stats.number_of_justice = j;
    _stats.number_of_fairness = f;
  }

  void on_output( unsigned index, unsigned lit ) const override
  {
    (void)index;
    (void)lit;
    _stats.outputs.emplace_back( std::make_tuple(index, lit) );
  }

  void on_and( unsigned index, unsigned left_lit, unsigned right_lit ) const override
  {
    (void)index;
    (void)left_lit;
    (void)right_lit;
    _stats.ands.emplace_back( std::make_tuple(index, left_lit, right_lit) );
  }

  void on_latch( unsigned index, unsigned next, latch_init_value init_value ) const override
  {
    _stats.latches.emplace_back( std::make_tuple(index, next, init_value) );
  }

  void on_bad_state( unsigned index, unsigned lit ) const override
  {
    _stats.bad_states.emplace_back( std::make_tuple(index, lit) );
  }

  void on_constraint( unsigned index, unsigned lit ) const override
  {
    _stats.constraints.emplace_back( std::make_tuple(index, lit) );
  }

  void on_justice( unsigned index, const std::vector<unsigned>& lits ) const override
  {
    _stats.justice.emplace_back( std::make_tuple(index, lits) );
  }

  void on_fairness( unsigned index, unsigned lit ) const override
  {
    _stats.fairness.emplace_back( std::make_tuple(index, lit) );
  }

  void on_input_name( unsigned index, const std::string& name ) const override
  {
    _stats.input_names.insert( std::make_pair( index, name ) );
  }

  void on_output_name( unsigned index, const std::string& name ) const override
  {
    _stats.output_names.insert( std::make_pair( index, name ) );
  }

  void on_comment( const std::string& comment ) const override
  {
    _stats.comment = comment;
  }

  aiger_statistics& _stats;
}; /* aiger_statistics_reader */

TEST_CASE( "combinational", "[aiger]" )
{
  char aiger_file[] = {
      0x61, 0x69, 0x67, 0x20, 0x36, 0x20, 0x32, 0x20, 0x30, 0x20, 0x32,
      0x20, 0x34, 0x0a, 0x31, 0x30, 0x0a, 0x31, 0x32, 0x0a, 0x02, 0x02,
      0x03, 0x02, 0x01, 0x02, 0x07, 0x03}; /* aiger_file */

  std::istringstream iss( aiger_file );

  aiger_statistics stats;
  aiger_statistics_reader reader( stats );

  auto result = read_aiger( iss, reader );
  CHECK( result == return_code::success );
  CHECK( stats.maximum_variable_index == 6 );
  CHECK( stats.number_of_inputs == 2 );
  CHECK( stats.number_of_latches == 0 );
  CHECK( stats.number_of_outputs == 2 );
  CHECK( stats.number_of_ands == 4 );
  CHECK( stats.ands.size()== stats.number_of_ands );
  CHECK( stats.outputs.size() == stats.number_of_outputs );
}

TEST_CASE( "symbol_table", "[aiger]" )
{
  char aiger_file[] = {
      0x61, 0x69, 0x67, 0x20, 0x35, 0x20, 0x32, 0x20, 0x30, 0x20,
      0x32, 0x20, 0x33, 0x0a, 0x31, 0x30, 0x0a, 0x36, 0x0a, 0x02,
      0x02, 0x03, 0x02, 0x01, 0x02, 0x69, 0x30, 0x20, 0x78, 0x0a,
      0x69, 0x31, 0x20, 0x79, 0x0a, 0x6f, 0x30, 0x20, 0x73, 0x0a,
      0x6f, 0x31, 0x20, 0x63, 0x0a, 0x63, 0x0a, 0x68, 0x61, 0x6c,
      0x66, 0x20, 0x61, 0x64, 0x64, 0x65, 0x72, 0x0a, 0x00};
      /* aiger_file */

  std::istringstream iss( aiger_file );

  aiger_statistics stats;
  aiger_statistics_reader reader( stats );

  auto result = read_aiger( iss, reader );
  CHECK( result == return_code::success );
  CHECK( stats.maximum_variable_index == 5 );
  CHECK( stats.number_of_inputs == 2 );
  CHECK( stats.number_of_latches == 0 );
  CHECK( stats.number_of_outputs == 2 );
  CHECK( stats.number_of_ands == 3 );
  CHECK( stats.ands.size() == stats.number_of_ands );
  CHECK( stats.outputs.size() == stats.number_of_outputs );

  CHECK( stats.comment == "half adder" );
  CHECK( stats.input_names[0] == "x" );
  CHECK( stats.input_names[1] == "y" );
  CHECK( stats.output_names[0] == "s" );
  CHECK( stats.output_names[1] == "c" );
}

TEST_CASE( "sequential", "[aiger]" )
{
  char aiger_file[] = {
      0x61, 0x69, 0x67, 0x20, 0x37, 0x20, 0x32, 0x20, 0x31, 0x20, 0x32,
      0x20, 0x34, 0x0a, 0x31, 0x34, 0x0a, 0x36, 0x0a, 0x37, 0x0a, 0x02,
      0x04, 0x03, 0x04, 0x01, 0x02, 0x02, 0x08}; /* aiger_file */

  std::istringstream iss( aiger_file );

  aiger_statistics stats;
  aiger_statistics_reader reader( stats );

  auto result = read_aiger( iss, reader );
  CHECK( result == return_code::success );
  CHECK( stats.maximum_variable_index == 7 );
  CHECK( stats.number_of_inputs == 2 );
  CHECK( stats.number_of_latches == 1 );
  CHECK( stats.number_of_outputs == 2 );
  CHECK( stats.number_of_ands == 4 );
  CHECK( stats.ands.size() == stats.number_of_ands );
  CHECK( stats.latches.size() == stats.number_of_latches );
  CHECK( stats.outputs.size() == stats.number_of_outputs );

  CHECK( std::get<2>( stats.latches[0u] ) == aiger_reader::latch_init_value::NONDETERMINISTIC );
}

TEST_CASE( "latch_initialization", "[aiger]" )
{
  char aiger_file[] = {
      0x61, 0x69, 0x67, 0x20, 0x37, 0x20, 0x32, 0x20, 0x31, 0x20, 0x32,
      0x20, 0x34, 0x0a, 0x31, 0x34, 0x20, 0x31, 0x0a, 0x36, 0x0a, 0x37,
      0x0a, 0x02, 0x04, 0x03, 0x04, 0x01, 0x02, 0x02, 0x08}; /* aiger_file */

  std::istringstream iss( aiger_file );

  aiger_statistics stats;
  aiger_statistics_reader reader( stats );

  auto result = read_aiger( iss, reader );
  CHECK( result == return_code::success );
  CHECK( stats.maximum_variable_index == 7 );
  CHECK( stats.number_of_inputs == 2 );
  CHECK( stats.number_of_latches == 1 );
  CHECK( stats.number_of_outputs == 2 );
  CHECK( stats.number_of_ands == 4 );
  CHECK( stats.ands.size() == stats.number_of_ands );
  CHECK( stats.latches.size() == stats.number_of_latches );
  CHECK( stats.outputs.size() == stats.number_of_outputs );

  CHECK( std::get<2>( stats.latches[0u] ) == aiger_reader::latch_init_value::ONE );
}

TEST_CASE( "ascii_format", "[aiger]" )
{
  std::string aiger_file =
    "aag 7 2 0 2 3\n"
    "2\n"
    "4\n"
    "6\n"
    "12\n"
    "6 13 15\n"
    "12 2 4\n"
    "14 3 5\n"
    "i0 x\n"
    "i1 y\n"
    "o0 s\n"
    "o1 c\n"
    "c\n"
    "half adder\n";

  std::istringstream iss( aiger_file );

  aiger_statistics stats;
  aiger_statistics_reader reader( stats );

  diagnostic_engine diag;
  auto result = read_ascii_aiger( iss, reader, &diag );
  CHECK( result == return_code::success );

  CHECK( stats.maximum_variable_index == 7 );
  CHECK( stats.number_of_inputs == 2 );
  CHECK( stats.number_of_latches == 0 );
  CHECK( stats.number_of_outputs == 2 );
  CHECK( stats.number_of_ands == 3 );
  CHECK( stats.ands.size() == stats.number_of_ands );
  CHECK( stats.latches.size() == stats.number_of_latches );
  CHECK( stats.outputs.size() == stats.number_of_outputs );
}

TEST_CASE( "ascii_format_sequential", "[aiger]" )
{
  std::string aiger_file =
    "aag 7 2 1 2 4\n"
    "2\n"
    "4\n"
    "6 8\n"
    "6\n"
    "7\n"
    "8 4 10\n"
    "10 13 15\n"
    "12 2 6\n"
    "14 3 7\n";

  std::istringstream iss( aiger_file );

  aiger_statistics stats;
  aiger_statistics_reader reader( stats );

  diagnostic_engine diag;
  auto result = read_ascii_aiger( iss, reader, &diag );
  CHECK( result == return_code::success );
  CHECK( stats.maximum_variable_index == 7 );
  CHECK( stats.number_of_inputs == 2 );
  CHECK( stats.number_of_latches == 1 );
  CHECK( stats.number_of_outputs == 2 );
  CHECK( stats.number_of_ands == 4 );
  CHECK( stats.ands.size() == stats.number_of_ands );
  CHECK( stats.latches.size() == stats.number_of_latches );
  CHECK( stats.outputs.size() == stats.number_of_outputs );
}

TEST_CASE( "ascii_format_bad_state", "[aiger]" )
{
  std::string aiger_file =
    "aag 5 1 1 0 3 1\n"
    "2\n"
    "4 10 0\n"
    "4\n"
    "6 5 3\n"
    "8 4 2\n"
    "10 9 7\n";

  std::istringstream iss( aiger_file );

  aiger_statistics stats;
  aiger_statistics_reader reader( stats );

  diagnostic_engine diag;
  auto result = read_ascii_aiger( iss, reader, &diag );
  CHECK( result == return_code::success );
  CHECK( stats.number_of_bad_states == 1u );
  CHECK( stats.bad_states.size() == stats.number_of_bad_states );
}

TEST_CASE( "ascii_format_invariants", "[aiger]" )
{
  std::string aiger_file =
    "aag 5 1 1 0 3 1 1\n"
    "2\n"
    "4 10 0\n"
    "4\n"
    "3\n"
    "6 5 3\n"
    "8 4 2\n"
    "10 9 7\n";

  std::istringstream iss( aiger_file );

  aiger_statistics stats;
  aiger_statistics_reader reader( stats );

  diagnostic_engine diag;
  auto result = read_ascii_aiger( iss, reader, &diag );
  CHECK( result == return_code::success );
  CHECK( stats.number_of_constraints == 1u );
  CHECK( stats.constraints.size() == stats.number_of_constraints );
}
