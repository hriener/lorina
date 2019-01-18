#include <catch.hpp>

#include <lorina/liberty.hpp>

using namespace lorina;

TEST_CASE( "example0", "[liberty]" )
{
  std::string liberty_file =
    "library (example0) {\n"
    "}\n";

  std::istringstream iss( liberty_file );
  liberty_reader reader;
  auto result = read_liberty( iss, reader );
  CHECK( result == return_code::success );
}

TEST_CASE( "example1", "[liberty]" )
{
  std::string liberty_file =
    "library (example1) {\n"
    "  technology (cmos);\n"
    "  delay_model : table_lookup;\n"
    "  date : \"January 18, 2019\";\n"
    "  comment : \"Copyright (C) 2018  EPFL\";\n"
    "  revision : 2019.01 ;\n"
    "  bus_naming_style : \"Bus%sPin%d\";\n"
    "}\n";

  std::istringstream iss( liberty_file );
  liberty_reader reader;
  auto result = read_liberty( iss, reader );
  CHECK( result == return_code::success );
}
