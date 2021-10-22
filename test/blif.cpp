#include <catch.hpp>

#include <lorina/blif.hpp>
#include <optional>
#include <sstream>
#include <string>

using namespace lorina;

struct blif_statistics
{
  uint32_t number_of_inputs{0};
  uint32_t number_of_outputs{0};
  uint32_t number_of_gates{0};
  uint32_t number_of_latches{0};
  bool end_seen = false;
  bool equal_size_check = true;
};

class blif_statistics_reader : public blif_reader
{
public:
  explicit blif_statistics_reader( blif_statistics& stats )
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

  virtual void on_latch( const std::string& input, const std::string& output, const std::optional<latch_type>& type, const std::optional<std::string>& control, const std::optional<latch_init_value>& reset ) const override
  {
    (void)input;
    (void)output;
    (void)type;
    (void)control;
    (void)reset;

    ++_stats.number_of_latches;
  }

  virtual void on_end() const override
  {
    _stats.end_seen = true;
  }

  blif_statistics& _stats;
}; /* blif_statistics_reader */

TEST_CASE( "Check return_code of read_blif", "[blif]")
{
  std::string broken_file =
    "model top\n"
    "names y0\n"
    " 0\n"
    "end\n";

  {
    std::istringstream iss( broken_file );
    CHECK( read_blif( iss, blif_reader{} ) == return_code::parse_error );
  }

  std::string correct_file =
    ".model top\n"
    ".inputs x0 x1\n"
    ".outputs y0\n"
    ".names y0\n"
    " 0\n"
    ".end\n";

  {
    std::istringstream iss( correct_file );
    CHECK( read_blif( iss, blif_reader{} ) == return_code::success );
  }
}

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

TEST_CASE( "parse large number of inputs", "[blif]" )
{
  std::string blif_file_begin =
      ".model test\n";

  std::string blif_file_end =
      ".outputs y0\n"
      ".names y0\n"
      " 1\n"
      ".end\n";

  for ( auto i = 0u; i <= 16; ++i )
  {
    // std::cout << "[i] try with " << ( 1 << i ) << " inputs" << std::endl;
    std::string input_declaration = ".inputs";
    for ( auto j = 0; j < ( 1 << i ); ++j )
    {
      input_declaration += fmt::format( " x{}", j );
    }
    input_declaration += "\n";

    std::istringstream iss( blif_file_begin + input_declaration + blif_file_end );

    blif_statistics stats;
    blif_statistics_reader reader( stats );
    auto result = read_blif( iss, reader );
    CHECK( result == return_code::success );
  }
}

TEST_CASE( "parse large number of outputs", "[blif]" )
{
  std::string blif_file_begin =
      ".model test\n"
      ".inputs x0 x1\n";

  std::string blif_file_end =
      ".names y0\n"
      " 1\n"
      ".end\n";

  for ( auto i = 0u; i <= 16; ++i )
  {
    // std::cout << "[i] try with " << ( 1 << i ) << " outputs" << std::endl;
    std::string output_declaration = ".outputs";
    for ( auto j = 0; j < ( 1 << i ); ++j )
    {
      output_declaration += fmt::format( " y{}", j );
    }
    output_declaration += "\n";

    std::istringstream iss( blif_file_begin + output_declaration + blif_file_end );

    blif_statistics stats;
    blif_statistics_reader reader( stats );
    auto result = read_blif( iss, reader );
    CHECK( result == return_code::success );
  }
}

TEST_CASE( "read a combinational BLIF file", "[blif]" )
{
  std::string file{
    ".model top\n"
    ".inputs a b c\n"
    ".outputs y1 y2\n"
    ".names a b n1\n"
    "11 1\n"
    ".names c n1 n2\n"
    "1- 1\n"
    "-1 1\n"
    ".names n2 y1\n"
    "0 1\n"
    ".names n2 y2\n"
    "1 1\n"
    ".end\n"};

  blif_statistics stats;
  blif_statistics_reader reader( stats );

  std::istringstream in( file );
  auto const result = lorina::read_blif( in, reader );

  /* structural checks */
  CHECK( result == lorina::return_code::success );
  CHECK( stats.number_of_gates == 4 );
  CHECK( stats.number_of_inputs == 3 );
  CHECK( stats.number_of_outputs == 2 );
  CHECK( stats.number_of_latches == 0 );
}

TEST_CASE( "read a sequential BLIF file with 5 parameter latches that is not in topological order", "[blif]" )
{
  std::string file{
    ".model top\n"
    ".inputs clock a b c d\n"
    ".outputs f\n"
    ".names li1 a li0\n"
    "01 1\n"
    ".names lo0 new_new_n19__ li1\n"
    "01 1\n"
    ".names a lo1 new_new_n15__\n"
    "01 1\n"
    ".names d new_new_n15__ new_new_n16__\n"
    "00 1\n"
    ".names b lo2 new_new_n17__\n"
    "00 1\n"
    ".names new_new_n15__ new_new_n17__ new_new_n18__\n"
    "00 1\n"
    ".names new_new_n16__ new_new_n18__ new_new_n19__\n"
    "00 1\n"
    ".names new_new_n19__ new_new_n17__ li3\n"
    "00 1\n"
    ".names lo3 lo4 new_new_n20__\n"
    "00 1\n"
    ".names new_new_n20__ new_new_n18__ li4\n"
    "00 1\n"
    ".names c new_new_n17__ li2\n"
    "00 1\n"
    ".names li1 li4 f\n"
    "00 1\n"
    ".latch li0 lo0 fe clock 0\n"
    ".latch li1 lo1 re clock 1\n"
    ".latch li2 lo2 ah clock 2\n"
    ".latch li3 lo3 al clock 3\n"
    ".latch li4 lo4 as clock 1\n"
    ".end\n"};

  blif_statistics stats;
  blif_statistics_reader reader( stats );

  std::istringstream in( file );
  auto const result = lorina::read_blif( in, reader );

  CHECK( result == lorina::return_code::success );
  CHECK( stats.number_of_gates == 12 );
  CHECK( stats.number_of_inputs == 5 );
  CHECK( stats.number_of_outputs == 1 );
  CHECK( stats.number_of_latches == 5 );
}

TEST_CASE( "read a BLIF file containing latch declaration bug that requires updated 'split' function", "[blif]" )
{
  std::string file{
    ".model top\n"
    ".inputs clock a b c d\n"
    ".outputs f\n"
    ".latch     lo0_in        lo0  1\n"
    ".latch     lo1_in        lo1  1\n"
    ".latch     lo2_in        lo2  1\n"
    ".names a lo1 new_n16_\n"
    "01 1\n"
    ".names d new_n16_ new_n17_\n"
    "00 1\n"
    ".names b lo2 new_n18_\n"
    "00 1\n"
    ".names new_n16_ new_n18_ new_n19_\n"
    "00 1\n"
    ".names new_n17_ new_n19_ new_n20_\n"
    "00 1\n"
    ".names lo0 new_n20_ lo1_in\n"
    "01 1\n"
    ".names a lo1_in lo0_in\n"
    "10 1\n"
    ".names c new_n18_ lo2_in\n"
    "00 1\n"
    ".names lo1_in f\n"
    "0 1\n"
    ".end\n"};

  blif_statistics stats;
  blif_statistics_reader reader( stats );

  std::istringstream in( file );
  auto const result = lorina::read_blif( in, reader );

  CHECK( result == lorina::return_code::success );
  CHECK( stats.number_of_gates == 9 );
  CHECK( stats.number_of_inputs == 5 );
  CHECK( stats.number_of_outputs == 1 );
  CHECK( stats.number_of_latches == 3 );
}
