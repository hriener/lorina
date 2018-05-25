#include <catch.hpp>

#include <lorina/liberty.hpp>

using namespace lorina;

TEST_CASE( "example1", "[liberty]" )
{
  std::string liberty_file =
    "library (example1) {\n"
    "  technology (cmos);\n"
    "  delay_model : table_lookup;\n"
    "  date : \"December 12, 2003\";\n"
    "  comment : \"Copyright 2003, General Silicon, Inc.\";\n"
    "  revision : 2003.12 ;\n"
    "  bus_naming_style : \"Bus%sPin%d\";\n"
    "}\n";

  std::istringstream iss( liberty_file );
  liberty_reader reader;
  auto result = read_liberty( iss, reader );
  CHECK( result == return_code::success );
}
