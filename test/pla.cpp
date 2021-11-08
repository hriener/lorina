#include <catch.hpp>

#include <lorina/pla.hpp>
#include <sstream>
#include <string>

using namespace lorina;

struct pla_statistics
{
  uint64_t number_of_inputs = 0;
  uint64_t number_of_outputs = 0;
  uint64_t number_of_terms = 0;
  uint64_t term_count = 0;
  bool end_seen = false;
  std::string type;
};

class pla_statistics_reader : public pla_reader
{
public:
  explicit pla_statistics_reader( pla_statistics& stats )
      : _stats( stats )
  {
  }

  void on_number_of_inputs( uint64_t number_of_inputs ) const override
  {
    _stats.number_of_inputs = number_of_inputs;
  }

  void on_number_of_outputs( uint64_t number_of_outputs ) const override
  {
    _stats.number_of_outputs = number_of_outputs;
  }

  void on_number_of_terms( uint64_t number_of_terms ) const override
  {
    _stats.number_of_terms = number_of_terms;
  }

  bool on_keyword( const std::string& keyword, const std::string& value ) const override
  {
    if ( keyword == "type" )
    {
      _stats.type = value;
      return true;
    }
    return false;
  }

  void on_end() const override
  {
    _stats.end_seen = true;
  }

  void on_term( const std::string&, const std::string& ) const override
  {
    ++_stats.term_count;
  }

  pla_statistics& _stats;
}; /* pla_statistics_reader */

TEST_CASE( "pla_parse", "[pla]" )
{
  std::string pla_file =
      ".i 4\n"
      ".o 1\n"
      ".p 4\n"
      "0000 1\n"
      "-1-1 1\n"
      "0-0- 1\n"
      "-011 1\n"
      ".e\n";

  std::istringstream iss( pla_file );

  pla_statistics stats;
  pla_statistics_reader reader( stats );
  auto result = read_pla( iss, reader );

  CHECK( result == return_code::success );
  CHECK( stats.number_of_inputs == 4 );
  CHECK( stats.number_of_outputs == 1 );
  CHECK( stats.number_of_terms == 4 );
  CHECK( stats.term_count == stats.number_of_terms );
  CHECK( stats.end_seen == true );
}

TEST_CASE( "esop_pla", "[pla]" )
{
  std::string pla_file =
      ".i 4\n"
      ".o 1\n"
      ".p 4\n"
      ".type esop\n"
      "0000 1\n"
      "-1-1 1\n"
      "0-0- 1\n"
      "-011 1\n"
      ".e\n";

  std::istringstream iss( pla_file );

  pla_statistics stats;
  pla_statistics_reader reader( stats );
  auto result = read_pla( iss, reader );

  CHECK( result == return_code::success );
  CHECK( stats.type == "esop" );
  CHECK( stats.number_of_inputs == 4 );
  CHECK( stats.number_of_outputs == 1 );
  CHECK( stats.number_of_terms == 4 );
  CHECK( stats.term_count == stats.number_of_terms );
  CHECK( stats.end_seen == true );
}

TEST_CASE( "diagnostics", "[pla]" )
{
  std::string pla_file =
      ".input 4\n"
      ".output 1\n"
      ".p 4\n"
      "0000 1\n"
      "-1-1\n"
      "0-0- 1\n"
      "-011 1\n"
      ".end\n";

  std::istringstream iss( pla_file );

  diagnostic_consumer consumer;
  diagnostic_engine diag( &consumer );
  pla_statistics stats;
  pla_statistics_reader reader( stats );
  auto result = read_pla( iss, reader, &diag );

  CHECK( result == return_code::parse_error );
  CHECK( diag.get_num_diagnostics() == 4 );
}

TEST_CASE( "2bit_adder", "[pla]" )
{
  std::string pla_file =
      "# 2-bit by 2-bit binary adder (with no carry input)\n"
      "  .i 4\n"
      "  .o 3\n"
      "  0000  000\n"
      "  0001  001\n"
      "  0010  010\n"
      "  0011  011\n"
      "  0100  001\n"
      "  0101  010\n"
      "  0110  011\n"
      "  0111  100\n"
      "  1000  010\n"
      "  1001  011\n"
      "  1010  100\n"
      "  1011  101\n"
      "  1100  011\n"
      "  1101  100\n"
      "  1110  101\n"
      "  1111  110\n"
      ".e\n";

  std::istringstream iss( pla_file );

  diagnostic_consumer consumer;
  diagnostic_engine diag( &consumer );
  pla_statistics stats;
  pla_statistics_reader reader( stats );
  auto result = read_pla( iss, reader, &diag );

  CHECK( result == return_code::success );
  CHECK( stats.number_of_inputs == 4 );
  CHECK( stats.number_of_outputs == 3 );
  CHECK( stats.term_count == 16 );
}
