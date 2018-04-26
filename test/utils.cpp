#include <catch.hpp>

#include <lorina/detail/utils.hpp>

using namespace lorina;

TEST_CASE( "basename", "[utils]" )
{
  std::string t( "/usr/home/test.cpp" );
  CHECK( "test.cpp" == detail::basename( t ) );
}
