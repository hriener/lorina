#include <catch.hpp>

#include <lorina/aiger.hpp>
#include <fmt/format.h>
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

TEST_CASE( "ascii_format_justice", "[aiger]" )
{
  /**
   * Generated with AIGER tools and NuSMV2:
   *   `smv2aig -L ltl2smv prio.smv > prio.aig; aigtoaig prio.aag`
   */
  std::string aiger_file =
    "aag 72 10 12 0 50 0 0 1\n"
    "2\n"
    "4\n"
    "6\n"
    "8\n"
    "10\n"
    "12\n"
    "14\n"
    "16\n"
    "18\n"
    "20\n"
    "22 2\n"
    "24 4\n"
    "26 6\n"
    "28 8\n"
    "30 10\n"
    "32 12\n"
    "34 14\n"
    "36 16\n"
    "38 18\n"
    "40 20\n"
    "42 122\n"
    "44 1\n"
    "3\n"
    "138\n"
    "142\n"
    "144\n"
    "46 8 6\n"
    "48 13 9\n"
    "50 48 4\n"
    "52 51 47\n"
    "54 11 2\n"
    "56 55 52\n"
    "58 56 15\n"
    "60 59 35\n"
    "62 58 34\n"
    "64 63 61\n"
    "66 10 9\n"
    "68 67 7\n"
    "70 69 31\n"
    "72 68 30\n"
    "74 73 71\n"
    "76 74 64\n"
    "78 49 33\n"
    "80 48 32\n"
    "82 81 79\n"
    "84 82 76\n"
    "86 17 2\n"
    "88 87 19\n"
    "90 89 39\n"
    "92 88 38\n"
    "94 93 91\n"
    "96 94 84\n"
    "98 87 37\n"
    "100 86 36\n"
    "102 101 99\n"
    "104 102 96\n"
    "106 105 41\n"
    "108 40 21\n"
    "110 109 107\n"
    "112 110 42\n"
    "114 113 44\n"
    "116 88 59\n"
    "118 117 21\n"
    "120 118 45\n"
    "122 121 115\n"
    "124 28 26\n"
    "126 33 29\n"
    "128 126 24\n"
    "130 129 125\n"
    "132 31 22\n"
    "134 133 130\n"
    "136 134 34\n"
    "138 137 42\n"
    "140 36 22\n"
    "142 141 42\n"
    "144 42 41\n"
    "i0 AIGER_NEXT_r_m\n"
    "i1 AIGER_NEXT_r_0\n"
    "i2 AIGER_NEXT_g_m\n"
    "i3 AIGER_NEXT_g_0\n"
    "i4 AIGER_NEXT_LTL_1_SPECF_8\n"
    "i5 AIGER_NEXT_LTL_1_SPECF_7\n"
    "i6 AIGER_NEXT_LTL_1_SPECF_5\n"
    "i7 AIGER_NEXT_LTL_1_SPECF_3\n"
    "i8 AIGER_NEXT_LTL_1_SPECF_1\n"
    "i9 AIGER_NEXT_IGNORE_LTL_1\n"
    "l0 r_m\n"
    "l1 r_0\n"
    "l2 g_m\n"
    "l3 g_0\n"
    "l4 LTL_1_SPECF_8\n"
    "l5 LTL_1_SPECF_7\n"
    "l6 LTL_1_SPECF_5\n"
    "l7 LTL_1_SPECF_3\n"
    "l8 LTL_1_SPECF_1\n"
    "l9 IGNORE_LTL_1\n"
    "l10 AIGER_VALID\n"
    "l11 AIGER_INITIALIZED\n"
    "j0 AIGER_JUST_0\n"
    "c\n"
    "smvtoaig\n"
    "1.9\n"
    "prio.smv\n";

  std::istringstream iss( aiger_file );

  aiger_statistics stats;
  aiger_statistics_reader reader( stats );

  diagnostic_engine diag;
  auto result = read_ascii_aiger( iss, reader, &diag );
  CHECK( result == return_code::success );
  CHECK( stats.number_of_justice == 1u );
  CHECK( stats.justice.size() == stats.number_of_justice );
}
