#include <catch.hpp>

#include <lorina/aiger.hpp>
#include <fmt/format.h>
#include <fmt/printf.h>
#include <map>
#include <iostream>

using namespace lorina;

struct aiger_statistics
{
  std::size_t maximum_variable_index = 0;
  std::size_t number_of_inputs = 0;
  std::size_t number_of_latches = 0;
  std::size_t number_of_outputs = 0;
  std::size_t number_of_ands = 0;

  std::size_t output_count = 0;
  std::size_t and_count = 0;
  std::size_t latch_count = 0;

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

  virtual void on_header( std::size_t m, std::size_t i, std::size_t l, std::size_t o, std::size_t a ) const override
  {
    _stats.maximum_variable_index = m;
    _stats.number_of_inputs = i;
    _stats.number_of_latches = l;
    _stats.number_of_outputs = o;
    _stats.number_of_ands = a;
  }

  virtual void on_output( unsigned index, unsigned lit ) const override
  {
    (void)index;
    (void)lit;
    ++_stats.output_count;
  }

  virtual void on_and( unsigned index, unsigned left_lit, unsigned right_lit ) const override
  {
    (void)index;
    (void)left_lit;
    (void)right_lit;
    ++_stats.and_count;
  }

  virtual void on_latch( unsigned index, unsigned next, latch_init_value init_value ) const override
  {
    latches.push_back( std::make_tuple(index,next,init_value) );
    ++_stats.latch_count;
  }

  virtual void on_input_name( unsigned index, const std::string& name ) const override
  {
    _stats.input_names.insert( std::make_pair( index, name ) );
  }

  virtual void on_output_name( unsigned index, const std::string& name ) const override
  {
    _stats.output_names.insert( std::make_pair( index, name ) );
  }

  virtual void on_comment( const std::string& comment ) const override
  {
    _stats.comment = comment;
  }

  aiger_statistics& _stats;
  mutable std::vector<std::tuple<unsigned,unsigned,latch_init_value>> latches;
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
  CHECK( stats.and_count == 4 );
  CHECK( stats.output_count == 2 );
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
  CHECK( stats.and_count == 3 );
  CHECK( stats.output_count == 2 );

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
  CHECK( stats.and_count == 4 );
  CHECK( stats.latch_count == 1 );
  CHECK( stats.output_count == 2 );

  CHECK( reader.latches.size() == 1u );
  CHECK( std::get<2>( reader.latches[0u] ) == aiger_reader::latch_init_value::NONDETERMINISTIC );
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
  CHECK( stats.and_count == 4 );
  CHECK( stats.latch_count == 1 );
  CHECK( stats.output_count == 2 );

  CHECK( reader.latches.size() == 1u );
  CHECK( std::get<2>( reader.latches[0u] ) == aiger_reader::latch_init_value::ONE );
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
  CHECK( stats.and_count == 3 );
  CHECK( stats.latch_count == 0 );
  CHECK( stats.output_count == 2 );
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
  CHECK( stats.and_count == 4 );
  CHECK( stats.latch_count == 1 );
  CHECK( stats.output_count == 2 );
}
