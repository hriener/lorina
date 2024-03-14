#include <catch.hpp>

#include <iostream>

#include <lorina/detail/asg.hpp>
#include <lorina/smt2.hpp>
#include <sstream>

using namespace lorina;

TEST_CASE( "declare function", "[smt2]" )
{
  std::string expr0{"(declare-fun x () Int)"};
  std::string expr1{"(declare-const x Int)"};

  asg<snode> g;
  // smt2_pretty_printer<asg<snode>> reader{g};
  smt2_reader<asg<snode>> reader{g};

  std::stringstream in;
  in << expr0;
  read_smt2( in, reader );

  CHECK( g.num_nodes() == 4u );
  CHECK( g[0].data == "x" ); CHECK( g[0].children.size() == 0u );
  CHECK( g[1].data == "()" ); CHECK( g[1].children.size() == 0u );
  CHECK( g[2].data == "Int" ); CHECK( g[2].children.size() == 0u );
  CHECK( g[3].data == "declare-fun" ); CHECK( g[3].children.size() == 3u );

  in.clear();
  in << expr1;
  read_smt2( in, reader );

  CHECK( g.num_nodes() == 7u );
  CHECK( g[4].data == "x" ); CHECK( g[4].children.size() == 0u );
  CHECK( g[5].data == "Int" ); CHECK( g[5].children.size() == 0u );
  CHECK( g[6].data == "declare-const" ); CHECK( g[6].children.size() == 2u );
}

TEST_CASE( "expressions", "[smt2]" )
{
  std::string expr0{"(and a b c)"};
  std::string expr1{"(and (or a b) c)"};
  std::string expr2{"(and a (or b c))"};
  std::string expr3{"(and a (or b c) d)"};

  asg<snode> g;
  // smt2_pretty_printer<asg<snode>> reader{g};
  smt2_reader<asg<snode>> reader{g};

  std::stringstream in;
  in << expr0;
  read_smt2( in, reader );

  CHECK( g.num_nodes() == 4u );
  CHECK( g[0].data == "a" ); CHECK( g[0].children.size() == 0u );
  CHECK( g[1].data == "b" );   CHECK( g[1].children.size() == 0u );
  CHECK( g[2].data == "c" );   CHECK( g[2].children.size() == 0u );
  CHECK( g[3].data == "and" );   CHECK( g[3].children.size() == 3u );

  in.clear();
  in << expr1;
  read_smt2( in, reader );

  CHECK( g.num_nodes() == 9u );
  CHECK( g[4].data == "a" );   CHECK( g[4].children.size() == 0u );
  CHECK( g[5].data == "b" );   CHECK( g[5].children.size() == 0u );
  CHECK( g[6].data == "or" );  CHECK( g[6].children.size() == 2u );
  CHECK( g[7].data == "c" );   CHECK( g[7].children.size() == 0u );
  CHECK( g[8].data == "and" ); CHECK( g[8].children.size() == 2u );

  in.clear();
  in << expr2;
  read_smt2( in, reader );

  CHECK( g.num_nodes() == 14u );
  CHECK( g[9].data == "a" );    CHECK( g[9].children.size() == 0u );
  CHECK( g[10].data == "b" );   CHECK( g[10].children.size() == 0u );
  CHECK( g[11].data == "c" );   CHECK( g[11].children.size() == 0u );
  CHECK( g[12].data == "or" );  CHECK( g[12].children.size() == 2u );
  CHECK( g[13].data == "and" ); CHECK( g[13].children.size() == 2u );

  in.clear();
  in << expr3;
  read_smt2( in, reader );

  CHECK( g.num_nodes() == 20u );
  CHECK( g[14].data == "a" );   CHECK( g[14].children.size() == 0u );
  CHECK( g[15].data == "b" );   CHECK( g[15].children.size() == 0u );
  CHECK( g[16].data == "c" );   CHECK( g[16].children.size() == 0u );
  CHECK( g[17].data == "or" );  CHECK( g[17].children.size() == 2u );
  CHECK( g[18].data == "d" );   CHECK( g[18].children.size() == 0u );
  CHECK( g[19].data == "and" ); CHECK( g[19].children.size() == 3u );
}
