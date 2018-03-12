#include <catch.hpp>

#include <lorina/blif.hpp>

using namespace lorina;

struct blif_statistics
{
  std::size_t number_of_inputs = 0;
  std::size_t number_of_outputs = 0;
  std::size_t number_of_gates = 0;
  bool end_seen = false;
  bool equal_size_check = true;
};

class blif_statistics_reader : public blif_reader
{
public:
  blif_statistics_reader( blif_statistics& stats )
      : _stats( stats )
  {
  }

  virtual void on_input( const std::string& name ) const override
  {
    (void)name;
    ++_stats.number_of_inputs;
  }

  virtual void on_output( const std::string& name ) const override
  {
    (void)name;
    ++_stats.number_of_outputs;
  }

  virtual void on_gate( const std::vector<std::string>& inputs, const std::string& output, const output_cover_t& cover ) const override
  {
    (void)inputs;
    (void)output;
    (void)cover;

    for ( const auto& c : cover )
    {
      if ( c.first.size() != inputs.size() )
      {
        _stats.equal_size_check = false;
      }
    }

    ++_stats.number_of_gates;
  }

  virtual void on_end() const override
  {
    _stats.end_seen = true;
  }

  blif_statistics& _stats;
}; /* blif_statistics_reader */

TEST_CASE( "blif_parse", "[blif]" )
{
  std::string blif_file =
      ".model test\n"
      ".inputs x0 x1 x2 x3\n"
      ".outputs y0 y1\n"
      ".names x0 x1 w0\n"
      "11 1\n"
      ".names w0 x2 w1\n"
      "01 1\n"
      "10 1\n"
      ".names w1 x2 x3 x0 y0\n"
      "1111 1\n"
      ".names y1\n"
      " 0\n"
      ".end\n";

  std::istringstream iss( blif_file );

  blif_statistics stats;
  blif_statistics_reader reader( stats );
  auto result = read_blif( iss, reader );
  CHECK( result == return_code::success );
  CHECK( stats.number_of_inputs == 4 );
  CHECK( stats.number_of_outputs == 2 );
  CHECK( stats.number_of_gates == 4 );
  CHECK( stats.end_seen == true );
}

TEST_CASE( "gate_types", "[blif]" )
{
  std::string blif_file =
      ".model test\n"
      ".inputs x0 x1 x2 x3\n"
      ".outputs y0 y1\n"
      ".names x0 x1 w0\n"
      "11 1\n"
      ".names w0 x2 w1\n"
      "01 0\n"
      "10 0\n"
      ".names w1 x2 x3 x0 y0\n"
      "1--1 1\n"
      ".names y1\n"
      " 0\n"
      ".end\n";

  std::istringstream iss( blif_file );

  blif_statistics stats;
  blif_statistics_reader reader( stats );
  auto result = read_blif( iss, reader );
  CHECK( result == return_code::success );
  CHECK( stats.number_of_inputs == 4 );
  CHECK( stats.number_of_outputs == 2 );
  CHECK( stats.number_of_gates == 4 );
  CHECK( stats.end_seen == true );
  CHECK( stats.equal_size_check == true );
}
