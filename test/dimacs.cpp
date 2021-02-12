#include <catch.hpp>

#include <lorina/dimacs.hpp>

using namespace lorina;

struct dimacs_statistics
{
  uint64_t number_of_variables = 0;
  uint64_t number_of_clauses = 0;
  std::vector<uint32_t> variable_appearance_count;
  std::string format;
};

class dimacs_statistics_reader : public dimacs_reader
{
public:
  dimacs_statistics_reader( dimacs_statistics& stats )
      : _stats( stats )
  {
  }

  void on_format( const std::string& format ) const override
  {
    _stats.format = format;
  }

  void on_number_of_variables( uint64_t number_of_variables ) const override
  {
    _stats.number_of_variables = number_of_variables;
    _stats.variable_appearance_count.resize(number_of_variables, 0);
  }

  void on_number_of_clauses( uint64_t number_of_clauses ) const override
  {
    _stats.number_of_clauses = number_of_clauses;
  }

  void on_clause( const std::vector<int>& clause ) const override
  {
    for ( auto lit : clause )
    {
      ++_stats.variable_appearance_count.at( std::abs( lit ) - 1 );
    }
  }

  dimacs_statistics& _stats;
}; /* dimacs_statistics_reader */

TEST_CASE( "cnf_dimacs", "[dimacs]" )
{
  std::string dimacs =
      "c\n"
      "c start with comments\n"
      "c\n"
      "c\n"
      "p cnf 5 3\n"
      "1 -5 4 0\n"
      "-1 5 3 4 0\n"
      "-3 -4 0\n";

  std::istringstream iss( dimacs );

  dimacs_statistics stats;
  dimacs_statistics_reader reader( stats );
  auto result = read_dimacs( iss, reader );

  CHECK( result == return_code::success );
  CHECK( stats.format == "cnf" );
  CHECK( stats.number_of_variables == 5 );
  CHECK( stats.number_of_clauses == 3 );
  std::vector<uint32_t> expected = { 2, 0, 2, 3, 2 };
  CHECK( stats.variable_appearance_count == expected );
}

TEST_CASE( "cnf_dimacs single line of clauses", "[dimacs]" )
{
  std::string dimacs =
      "c\n"
      "c start with comments\n"
      "c\n"
      "c\n"
      "p cnf 5 3\n"
      "1 -5 4 0 "
      "-1 5 3 4 0 "
      "-3 -4 0\n";

  std::istringstream iss( dimacs );

  dimacs_statistics stats;
  dimacs_statistics_reader reader( stats );
  auto result = read_dimacs( iss, reader );

  CHECK( result == return_code::success );
  CHECK( stats.format == "cnf" );
  CHECK( stats.number_of_variables == 5 );
  CHECK( stats.number_of_clauses == 3 );
  std::vector<uint32_t> expected = { 2, 0, 2, 3, 2 };
  CHECK( stats.variable_appearance_count == expected );
}

TEST_CASE( "cnf_dimacs missing problem specification", "[dimacs]" )
{
  std::string dimacs =
      "c\n"
      "c start with comments\n"
      "c\n"
      "c\n"
      "1 -5 4 0 "
      "-1 5 3 4 0 "
      "-3 -4 0\n";

  std::istringstream iss( dimacs );

  dimacs_statistics stats;
  dimacs_statistics_reader reader( stats );
  auto result = read_dimacs( iss, reader );

  CHECK( result == return_code::parse_error );
}

TEST_CASE( "cnf_dimacs missing clause delimiter", "[dimacs]" )
{
  std::string dimacs =
      "c\n"
      "c start with comments\n"
      "c\n"
      "c\n"
      "1 -5 4\n"
      "-1 5 3 4 0\n"
      "-3 -4 0\n";

  std::istringstream iss( dimacs );

  dimacs_statistics stats;
  dimacs_statistics_reader reader( stats );
  auto result = read_dimacs( iss, reader );

  CHECK( result == return_code::parse_error );
}
