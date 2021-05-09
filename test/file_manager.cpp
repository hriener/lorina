#include <catch.hpp>

#include <lorina/file_manager.hpp>
#include <iostream>

using namespace lorina;

TEST_CASE( "create file manager", "[file_manager]" )
{
  file_manager fm;
  auto fe  = fm.get_file( "../include/lorina/blif.hpp" );
  std::cout << fe->get_unique_id() << std::endl;

  auto fe2 = fm.get_file( "../include/lorina/blif.hpp" );
  std::cout << fe2->get_unique_id() << std::endl;

  auto fe3 = fm.get_file( "../include/lorina/file_manager.hpp" );
  std::cout << fe3->get_unique_id() << std::endl;
}
