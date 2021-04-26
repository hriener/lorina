#include <catch.hpp>

#include <lorina/verilog.hpp>

using namespace lorina;

/*! \brief A consumer for diagnostics. */
class test_diagnostics : public diagnostic_consumer
{
public:
  explicit test_diagnostics( std::vector<std::pair<diagnostic_level, std::string>>& logged_diags )
    : logged_diags_( logged_diags )
  {
  }

  void handle_diagnostic( diagnostic_level level, std::string const& message ) const override
  {
    logged_diags_.emplace_back( level, message );
  }

public:
  std::vector<std::pair<diagnostic_level, std::string>>& logged_diags_;
};

TEST_CASE( "Emit error", "[diagnostics]")
{
  std::vector<std::pair<diagnostic_level, std::string>> logged_diags;
  test_diagnostics consumer( logged_diags );
  diagnostic_engine diag( &consumer );
  diag.report( diag_id::ERR_PARSE_LINE ).add_argument( "a line to be parsed" );

  CHECK( logged_diags.size() == 1u );
  CHECK( logged_diags[0u].first == diagnostic_level::fatal );
  CHECK( logged_diags[0u].second == "could not parse line `a line to be parsed`" );
}

TEST_CASE( "Emit text diagnostics", "[diagnostics]")
{
  text_diagnostics consumer;
  diagnostic_engine diag( &consumer );

  auto const err_custom0 = diag.create_id( diagnostic_level::ignore, "ignore" );
  auto const err_custom1 = diag.create_id( diagnostic_level::note, "note" );
  auto const err_custom2 = diag.create_id( diagnostic_level::remark, "remark" );
  auto const err_custom3 = diag.create_id( diagnostic_level::warning, "warning" );
  auto const err_custom4 = diag.create_id( diagnostic_level::error, "error" );
  auto const err_custom5 = diag.create_id( diagnostic_level::fatal, "fatal" );
  diag.report( err_custom0 );
  diag.report( err_custom1 );
  diag.report( err_custom2 );
  diag.report( err_custom3 );
  diag.report( err_custom4 );
  diag.report( err_custom5 );
}

TEST_CASE( "Emit custom diagnostics", "[diagnostics]")
{
  std::vector<std::pair<diagnostic_level, std::string>> logged_diags;
  test_diagnostics consumer( logged_diags );
  diagnostic_engine diag( &consumer );

  auto const err_custom0 = diag.create_id( diagnostic_level::ignore, "ignore" );
  auto const err_custom1 = diag.create_id( diagnostic_level::note, "note" );
  auto const err_custom2 = diag.create_id( diagnostic_level::remark, "remark" );
  auto const err_custom3 = diag.create_id( diagnostic_level::warning, "warning" );
  auto const err_custom4 = diag.create_id( diagnostic_level::error, "error" );
  auto const err_custom5 = diag.create_id( diagnostic_level::fatal, "fatal" );
  diag.report( err_custom0 );
  diag.report( err_custom1 );
  diag.report( err_custom2 );
  diag.report( err_custom3 );
  diag.report( err_custom4 );
  diag.report( err_custom5 );

  CHECK( logged_diags.size() == 6u );
  CHECK( logged_diags[0u].first == diagnostic_level::ignore );
  CHECK( logged_diags[0u].second == "ignore" );
  CHECK( logged_diags[1u].first == diagnostic_level::note );
  CHECK( logged_diags[1u].second == "note" );
  CHECK( logged_diags[2u].first == diagnostic_level::remark );
  CHECK( logged_diags[2u].second == "remark" );
  CHECK( logged_diags[3u].first == diagnostic_level::warning );
  CHECK( logged_diags[3u].second == "warning" );
  CHECK( logged_diags[4u].first == diagnostic_level::error );
  CHECK( logged_diags[4u].second == "error" );
  CHECK( logged_diags[5u].first == diagnostic_level::fatal );
  CHECK( logged_diags[5u].second == "fatal" );
}

TEST_CASE( "Emit diagnostics with arguments", "[diagnostics]")
{
  std::vector<std::pair<diagnostic_level, std::string>> logged_diags;
  test_diagnostics consumer( logged_diags );
  diagnostic_engine diag( &consumer );

  /* emit static diagnostics */
  diag.report( diag_id::ERR_PARSE_LINE )
    .add_argument( "A" );

  diag.report( diag_id::WRN_UNRESOLVED_DEPENDENCY )
    .add_argument( "A" )
    .add_argument( "B" );

  /* emit custom diagnostics */
  auto const err_custom0 = diag.create_id( diagnostic_level::ignore, "ignore with {}" );
  auto const err_custom1 = diag.create_id( diagnostic_level::ignore, "ignore with {} and {}" );
  auto const err_custom2 = diag.create_id( diagnostic_level::ignore, "ignore with {}, {}, and {}" );
  diag.report( err_custom0 )
    .add_argument( "A" );
  diag.report( err_custom1 )
    .add_argument( "A" )
    .add_argument( "B" );
  diag.report( err_custom2 )
    .add_argument( "A" )
    .add_argument( "B" )
    .add_argument( "C" );
}
