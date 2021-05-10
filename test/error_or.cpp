#include <catch.hpp>

#include <lorina/error_or.hpp>
#include <string>
#include <memory>

TEST_CASE( "Error or integer", "[error]")
{
  lorina::error_or<int> e_int{42};
  CHECK( *e_int == 42 );
}

TEST_CASE( "Error or string", "[error]")
{
  lorina::error_or<std::string> e_string{"not an error"};
  CHECK( *e_string == "not an error" );
}

TEST_CASE( "Error or unique_ptr<int>", "[error]")
{
  lorina::error_or<std::unique_ptr<int>>
    e_int{std::make_unique<int>( 42 )};
  CHECK( **e_int == 42 );
}

TEST_CASE( "Error or unique_ptr<std::string>", "[error]")
{
  lorina::error_or<std::unique_ptr<std::string>>
    e_string{std::make_unique<std::string>( "not an error" )};
  CHECK( **e_string == "not an error" );
}

TEST_CASE( "Error or int&", "[error]")
{
  int i = 42;
  lorina::error_or<int&> e_int{i};
  *e_int += 25;
  CHECK( *e_int == 25 + 42 );
  CHECK( *e_int == i );
  std::cout << i << std::endl;
}
