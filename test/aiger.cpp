#include <catch.hpp>

#include <lorina/aiger.hpp>

#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <optional>

namespace benchmarks
{
  /* AIGs in binary AIGER format */
  std::vector<std::string> aig_benchmarks{
    "empty", "false", "true", "buffer", "inverter", "and", "half_adder", "toggle_ff", "toggle_ff2",
    "prio", "counter", "invariant"
  };

  /* AIGs in ASCII AIGER format */
  std::vector<std::string> aag_benchmarks{
    "empty", "false", "true", "buffer", "inverter", "and", "half_adder", "toggle_ff", "toggle_ff2",
    "prio", "counter", "invariant"
  };

  std::string path( std::string const& subpath, std::string const& benchmark_name, std::string const& extension )
  {
#ifndef BENCHMARKS_PATH
    return fmt::format( "{}/{}.{}", subpath, benchmark_name, extension );
#else
    return fmt::format( "{}benchmarks/{}/{}.{}", BENCHMARKS_PATH, subpath, benchmark_name, extension );
#endif
  }

  std::string path( std::string const& benchmark_name, std::string const& extension )
  {
#ifndef BENCHMARKS_PATH
    return fmt::format( "{}.{}", benchmark_name, extension );
#else
    return fmt::format( "{}benchmarks/{}.{}", BENCHMARKS_PATH, benchmark_name, extension );
#endif
  }

  struct aig_info
  {
    /* header information */
    uint64_t maximum_variable_index{0};
    uint64_t number_of_inputs{0};
    uint64_t number_of_latches{0};
    uint64_t number_of_outputs{0};
    uint64_t number_of_ands{0};

    uint64_t number_of_bad_states{0};
    uint64_t number_of_constraints{0};
    uint64_t number_of_justice{0};
    uint64_t number_of_fairness{0};

    /* count calls */
    uint32_t on_input_calls{0};
    uint32_t on_output_calls{0};
    uint32_t on_and_calls{0};
    uint32_t on_latch_calls{0};

    uint32_t on_bad_state_calls{0};
    uint32_t on_constraints_calls{0};
    uint32_t on_justice_calls{0};
    uint32_t on_fairness_calls{0};

    /* indices */
    std::vector<std::optional<uint32_t>> indices;

    /* metadata */
    nlohmann::json metadata;

    void reset()
    {
      *this = {};
    }

    bool check_all_indices_defined() const
    {
      for ( const auto& i : indices )
      {
        if ( !i )
        {
          return false;
        }
      }
      return true;
    }

    bool check_header_info() const
    {
      return maximum_variable_index == number_of_inputs + number_of_latches + number_of_ands + 1u &&
        on_input_calls == number_of_inputs &&
        on_output_calls == number_of_outputs &&
        on_and_calls == number_of_ands &&
        on_latch_calls == number_of_latches &&
        on_bad_state_calls == number_of_bad_states &&
        on_constraints_calls == number_of_constraints &&
        on_justice_calls == number_of_justice &&
        on_fairness_calls == number_of_justice;
    }
  }; /* aig_info */

  struct test_reader : public lorina::aiger_reader
  {
  public:
    explicit test_reader( aig_info& info )
      : info( info )
    {
    }

    void on_header( uint64_t m, uint64_t i, uint64_t l, uint64_t o, uint64_t a,
                    uint64_t b, uint64_t c, uint64_t j, uint64_t f ) const override
    {
      /* update header information in info struct */
      info.maximum_variable_index = m;
      info.number_of_inputs = i;
      info.number_of_latches = l;
      info.number_of_outputs = o;
      info.number_of_ands = a;
      info.number_of_bad_states = b;
      info.number_of_constraints = c;
      info.number_of_justice = j;
      info.number_of_fairness = f;

      /* reserve extra space for a constant 0 at index 0 */
      info.indices.resize( m+1, std::nullopt );
      info.indices[0] = 0;
      for ( auto index = 1u; index <= i; ++index )
      {
        info.indices[index] = index;
      }
    }

    void on_input( uint32_t pos, uint32_t lit ) const override
    {
      (void)pos;
      (void)lit;

      ++info.on_input_calls;
    }

    void on_output( uint32_t pos, uint32_t lit ) const override
    {
      (void)pos;
      (void)lit;

      ++info.on_output_calls;
    }

    void on_and( uint32_t index, uint32_t left_lit, uint32_t right_lit ) const override
    {
      (void)left_lit;
      (void)right_lit;

      ++info.on_and_calls;

       /* ensure that this index is not already in use */
      CHECK( !info.indices[index] );

      info.indices[index] = index;
    }

    void on_latch( uint32_t index, uint32_t next_lit, latch_init_value init_value ) const override
    {
      (void)next_lit;
      (void)init_value;

      ++info.on_latch_calls;

      /* ensure that this index is not already in use */
      CHECK( !info.indices[index] );

      info.indices[index] = index;
    }

    void on_bad_state( uint32_t pos, uint32_t lit ) const override
    {
      (void)pos;
      (void)lit;

      ++info.on_bad_state_calls;
    }

    void on_constraint( uint32_t pos, uint32_t lit ) const override
    {
      (void)pos;
      (void)lit;

      ++info.on_constraints_calls;
    }

    void on_justice( uint32_t pos, std::vector<uint32_t> const& lits ) const override
    {
      (void)pos;
      (void)lits;

      ++info.on_justice_calls;
    }

    void on_fairness( uint32_t pos, uint32_t lit ) const override
    {
      (void)pos;
      (void)lit;

      ++info.on_fairness_calls;
    }

    void on_comment( std::string const& comment ) const override
    {
      /* setup metadata by interpreting comment as JSON file */
      std::stringstream iss{comment};
      iss >> info.metadata;
    }

    aig_info& info;
  }; /* test_reader */
} /* namespace benchmarks */

TEST_CASE( "test ASCII Aiger reader", "[aiger]" )
{
  for ( const auto& benchmark : benchmarks::aag_benchmarks )
  {
    std::string const benchmark_path = benchmarks::path( "aag", benchmark, "aag" );

    /* read benchmark with default reader (increases the test coverage for the default visitor)*/
    {
      lorina::text_diagnostics consumer;
      lorina::diagnostic_engine diag( &consumer );
      auto const result = lorina::read_ascii_aiger( benchmark_path, lorina::aiger_reader{}, &diag );
      CHECK( result == lorina::return_code::success );
    }

    /* read benchmarks with test reader */
    {
      lorina::text_diagnostics consumer;
      lorina::diagnostic_engine diag( &consumer );
      benchmarks::aig_info info;
      auto const result = lorina::read_ascii_aiger( benchmark_path, benchmarks::test_reader{info}, &diag );
      CHECK( result == lorina::return_code::success );

      /* check if all indices are defined */
      if ( info.metadata.count( "disable_checks" ) && info.metadata["disable_checks"].count( "all_indices_defined" ) )
      {
        auto const result = info.check_all_indices_defined();
        if ( !result )
        {
          fmt::print( "[e] benchmark `{}`: all indices defined\n", benchmark );
        }
      }

      /* check if header is correct */
      if ( info.metadata.count( "disable_checks" ) && info.metadata["disable_checks"].count( "header_info" ) )
      {
        auto const result = info.check_header_info();
        if ( !result )
        {
          fmt::print( "[e] benchmark `{}`: header info\n", benchmark );
        }
      }
    }
  }
}

TEST_CASE( "test binary Aiger reader", "[aiger]" )
{
  for ( const auto& benchmark : benchmarks::aig_benchmarks )
  {
    std::string const benchmark_path = benchmarks::path( "aig", benchmark, "aig" );

    /* read benchmark with default reader (increases the test coverage for the default visitor)*/
    {
      lorina::text_diagnostics consumer;
      lorina::diagnostic_engine diag( &consumer );
      auto const result = lorina::read_aiger( benchmark_path, lorina::aiger_reader{}, &diag );
      CHECK( result == lorina::return_code::success );
    }

    /* read benchmarks with test reader */
    {
      lorina::text_diagnostics consumer;
      lorina::diagnostic_engine diag( &consumer );
      benchmarks::aig_info info;
      auto const result = lorina::read_aiger( benchmark_path, benchmarks::test_reader{info}, &diag );
      CHECK( result == lorina::return_code::success );

      /* check if header is correct */
      if ( info.metadata.count( "disable_checks" ) && info.metadata["disable_checks"].count( "header_info" ) )
      {
        auto const result = info.check_header_info();
        if ( !result )
        {
          fmt::print( "[e] benchmark `{}`: header info\n", benchmark );
        }
      }
    }
  }
}
