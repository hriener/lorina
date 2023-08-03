#include <catch.hpp>

#include <lorina/super.hpp>
#include <sstream>
#include <string>

using namespace lorina;

struct supergate_spec
{
  unsigned int id;
  std::string name;
  bool is_super;
  std::vector<uint32_t> fanin_id;
};

struct super_lib
{
  std::string genlib_name{};
  uint32_t max_num_vars{};
  uint32_t num_supergates{};
  uint32_t num_lines{};
  std::vector<supergate_spec> supergates{};
};

struct test_super_reader : public super_reader
{
public:
  explicit test_super_reader( super_lib& lib )
    : lib( lib )
  {}

  void on_super_info( std::string const& genlib_name, uint32_t max_num_vars, uint32_t max_supergates, uint32_t num_lines ) const
  {
    lib.genlib_name = genlib_name;
    lib.max_num_vars = max_num_vars;
    lib.num_supergates = max_supergates;
    lib.num_lines = num_lines;
  }

  void on_supergate( std::string const& name, bool const& is_super, std::vector<uint32_t> const& fanins_id ) const
  {
    lib.supergates.emplace_back( supergate_spec{ static_cast<unsigned int>( lib.supergates.size() ),
                                                 name,
                                                 is_super,
                                                 fanins_id } );
  }

public:
  super_lib& lib;
};

TEST_CASE( "instantiate super_reader", "[super]")
{
  std::string const super_file =
    "# comment\n"
    "test.genlib\n"
    "3\n"
    "8\n"
    "14\n"
    "* and 1 0\n"
    "* and 2 3\n"
    "and 2 0\n"
    "* and 1 5\n"
    "or 2 1\n"
    "* and 0 7\n"
    "* or 1 0\n"
    "* and 0 9\n"
    "or 2 0\n"
    "* and 1 11\n"
    "* and 2 9\n"
    "\0"
    ;

  text_diagnostics consumer;
  diagnostic_engine diag( &consumer );
  std::istringstream iss( super_file );
  super_lib super_definition{};
  CHECK( read_super( iss, super_reader{}, &diag ) == return_code::success );
}

TEST_CASE( "super error cases", "[super]" )
{
  {
    /* not all required fields have been specified */
    std::string const super_file =
      "test.genlib\n"
      "3\n"
      "8\n"
      "* and 1 0\n"
      ;
    std::istringstream iss( super_file );
    diagnostic_consumer consumer;
    diagnostic_engine diag( &consumer );
    CHECK( read_super( iss, super_reader{}, &diag ) == return_code::parse_error );
  }

  {
    /* super not completed */
    std::string const super_file =
      "test.genlib\n"
      "3\n"
      "8\n1"
      ;
    std::istringstream iss( super_file );
    diagnostic_consumer consumer;
    diagnostic_engine diag( &consumer );
    CHECK( read_super( iss, super_reader{}, &diag ) == return_code::success );
  }

  {
    /* end of file delimiter */
    std::string const super_file = "\0";
    std::istringstream iss( super_file );
    diagnostic_consumer consumer;
    diagnostic_engine diag( &consumer );
    CHECK( read_super( iss, super_reader{}, &diag ) == return_code::success );
  }

  {
    /* wrong format */
    std::string const super_file =
      "test.genlib\n"
      "3\n"
      "8\n"
      "14\n"
      "and"
      ;
    std::istringstream iss( super_file );
    diagnostic_consumer consumer;
    diagnostic_engine diag( &consumer );
    CHECK( read_super( iss, super_reader{}, &diag ) == return_code::parse_error );
  }

  {
    /* exeeding max number of variables */
    std::string const super_file =
      "test.genlib\n"
      "2\n"
      "8\n"
      "14\n"
      "* and3 0 1 2"
      ;
    std::istringstream iss( super_file );
    diagnostic_consumer consumer;
    diagnostic_engine diag( &consumer );
    CHECK( read_super( iss, super_reader{}, &diag ) == return_code::parse_error );
  }

  {
    /* missing fanin */
    std::string const super_file =
      "test.genlib\n"
      "2\n"
      "8\n"
      "14\n"
      "* and3"
      ;
    std::istringstream iss( super_file );
    diagnostic_consumer consumer;
    diagnostic_engine diag( &consumer );
    CHECK( read_super( iss, super_reader{}, &diag ) == return_code::parse_error );
  }
}

TEST_CASE( "read SUPER format", "[super]")
{
  std::string const super_file =
    "# comment\n"
    "test.genlib\n"
    "3\n"
    "2\n"
    "6\n"
    "* and2 1 0\n"
    "* or2 2 1\n"
    "and2 2 0\n"
    ;

  text_diagnostics consumer;
  diagnostic_engine diag( &consumer );
  std::istringstream iss( super_file );
  super_lib super_definition{};
  test_super_reader reader( super_definition );
  CHECK( read_super( iss, reader, &diag ) == return_code::success );

  CHECK( super_definition.genlib_name == "test.genlib" );
  CHECK( super_definition.max_num_vars == 3 );
  CHECK( super_definition.num_supergates == 2 );
  CHECK( super_definition.num_lines == 6 );
  CHECK( super_definition.supergates.size() == 3 );

  CHECK( super_definition.supergates[0].id == 0 );
  CHECK( super_definition.supergates[0].name == "and2" );
  CHECK( super_definition.supergates[0].is_super == true );
  CHECK( super_definition.supergates[0].fanin_id.size() == 2 );
  CHECK( super_definition.supergates[0].fanin_id[0] == 1 );
  CHECK( super_definition.supergates[0].fanin_id[1] == 0 );

  CHECK( super_definition.supergates[1].id == 1 );
  CHECK( super_definition.supergates[1].name == "or2" );
  CHECK( super_definition.supergates[1].is_super == true );
  CHECK( super_definition.supergates[1].fanin_id.size() == 2 );
  CHECK( super_definition.supergates[1].fanin_id[0] == 2 );
  CHECK( super_definition.supergates[1].fanin_id[1] == 1 );

  CHECK( super_definition.supergates[2].id == 2 );
  CHECK( super_definition.supergates[2].name == "and2" );
  CHECK( super_definition.supergates[2].is_super == false );
  CHECK( super_definition.supergates[2].fanin_id.size() == 2 );
  CHECK( super_definition.supergates[2].fanin_id[0] == 2 );
  CHECK( super_definition.supergates[2].fanin_id[1] == 0 );
}
