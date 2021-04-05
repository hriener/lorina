#include <catch.hpp>

#include <lorina/verilog.hpp>

using namespace lorina;

TEST_CASE( "Emit messages", "[diagnostics]")
{
  diagnostic_engine engine;
  engine.report( diagnostic_level::ignore, "ignore" );
  engine.report( diagnostic_level::note, "note" );
  engine.report( diagnostic_level::remark, "remark" );
  engine.report( diagnostic_level::warning, "warning" );
  engine.report( diagnostic_level::error, "error" );
  engine.report( diagnostic_level::fatal, "fatal" );
}
