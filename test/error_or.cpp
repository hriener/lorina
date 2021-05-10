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
}

TEST_CASE( "Copy construct error_or", "[error]" )
{
  int i = 42;
  lorina::error_or<int&> e_int( i );
  lorina::error_or<int&> e_int_copy( e_int );
  i += 10;
  CHECK( *e_int == 52 );
  CHECK( *e_int_copy == 52 );
}

TEST_CASE( "Move construct error_or", "[error]" )
{
  int i = 42;
  lorina::error_or<int&> e_int( i );
  lorina::error_or<int&> e_int_copy( std::move( e_int ) );
  i += 10;
  CHECK( *e_int == 52 );
  CHECK( *e_int_copy == 52 );
}

TEST_CASE( "Copy assign error_or", "[error]" )
{
  int i = 42;
  int j = 21;
  lorina::error_or<int&> e_int( i );
  lorina::error_or<int&> e_int2( j );
  e_int2 = e_int;
  *e_int2 += 10;
  CHECK( *e_int2 == 52 );
  CHECK( i == 52 );
}

TEST_CASE( "Move assign error_or", "[error]" )
{
  int i = 42;
  int j = 21;
  lorina::error_or<int&> e_int( i );
  lorina::error_or<int&> e_int2( j );
  e_int2 = std::move( e_int );
  *e_int2 += 10;
  CHECK( *e_int2 == 52 );
  CHECK( i == 52 );
}
