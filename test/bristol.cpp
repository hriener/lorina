#include <catch.hpp>

#include <lorina/bristol.hpp>
#include <iostream>
#include <sstream>

class bristol_test_reader : public lorina::bristol_reader
{
public:
  explicit bristol_test_reader() {}

  virtual void on_header( uint32_t num_gates, uint32_t num_wires, uint32_t num_inputs, std::vector<uint32_t> const& num_wires_per_input, uint32_t num_outputs, std::vector<uint32_t> const& num_wires_per_output ) const override
  {
    std::ostringstream s;
    s << num_gates << " " << num_wires << " " << num_inputs << " " << num_wires_per_input.size() << " " << num_outputs << " " << num_wires_per_output.size() << std::endl;
  }

  virtual void on_gate( std::vector<uint32_t> const& in, uint32_t out, std::string const& gate ) const override
  {
    std::ostringstream s;
    s << out << " = " << gate << " ";
    for ( const auto& i : in )
    {
      s << i << ' ';
    }
    s << std::endl;
  }
};

TEST_CASE( "Read MAJ as Bristol file", "[bristol]")
{
  std::string const maj
  {
    "4 7\n"
    "3 1 1 1\n"
    "1 1\n"
    "\n"
    "2 1 0 1 3 XOR\n"
    "2 1 1 2 4 XOR\n"
    "2 1 3 4 5 AND\n"
    "2 1 1 5 6 XOR\n"
  };

  std::stringstream ss; ss << maj;

  bristol_test_reader reader;
  auto result{lorina::read_bristol( ss, reader )};
  CHECK( result == lorina::return_code::success );
}

TEST_CASE( "Read Bristol file", "[bristol]")
{
  std::string const zero_equal
  {
    "127 191\n"
    "1 64\n"
    "1 1\n"
    "\n"
    "1 1 63 65 INV\n"
    "1 1 60 64 INV\n"
    "2 1 65 64 69 AND\n"
    "1 1 62 67 INV\n"
    "1 1 61 66 INV\n"
    "2 1 67 66 68 AND\n"
    "2 1 69 68 77 AND\n"
    "1 1 51 71 INV\n"
    "1 1 48 70 INV\n"
    "2 1 71 70 75 AND\n"
    "1 1 50 73 INV\n"
    "1 1 49 72 INV\n"
    "2 1 73 72 74 AND\n"
    "2 1 75 74 76 AND\n"
    "2 1 77 76 93 AND\n"
    "1 1 59 79 INV\n"
    "1 1 56 78 INV\n"
    "2 1 79 78 83 AND\n"
    "1 1 58 81 INV\n"
    "1 1 57 80 INV\n"
    "2 1 81 80 82 AND\n"
    "2 1 83 82 91 AND\n"
    "1 1 55 85 INV\n"
    "1 1 52 84 INV\n"
    "2 1 85 84 89 AND\n"
    "1 1 54 87 INV\n"
    "1 1 53 86 INV\n"
    "2 1 87 86 88 AND\n"
    "2 1 89 88 90 AND\n"
    "2 1 91 90 92 AND\n"
    "2 1 93 92 125 AND\n"
    "1 1 15 95 INV\n"
    "1 1 12 94 INV\n"
    "2 1 95 94 99 AND\n"
    "1 1 14 97 INV\n"
    "1 1 13 96 INV\n"
    "2 1 97 96 98 AND\n"
    "2 1 99 98 107 AND\n"
    "1 1 3 101 INV\n"
    "1 1 0 100 INV\n"
    "2 1 101 100 105 AND\n"
    "1 1 2 103 INV\n"
    "1 1 1 102 INV\n"
    "2 1 103 102 104 AND\n"
    "2 1 105 104 106 AND\n"
    "2 1 107 106 123 AND\n"
    "1 1 11 109 INV\n"
    "1 1 8 108 INV\n"
    "2 1 109 108 113 AND\n"
    "1 1 10 111 INV\n"
    "1 1 9 110 INV\n"
    "2 1 111 110 112 AND\n"
    "2 1 113 112 121 AND\n"
    "1 1 7 115 INV\n"
    "1 1 4 114 INV\n"
    "2 1 115 114 119 AND\n"
    "1 1 6 117 INV\n"
    "1 1 5 116 INV\n"
    "2 1 117 116 118 AND\n"
    "2 1 119 118 120 AND\n"
    "2 1 121 120 122 AND\n"
    "2 1 123 122 124 AND\n"
    "2 1 125 124 189 AND\n"
    "1 1 47 127 INV\n"
    "1 1 44 126 INV\n"
    "2 1 127 126 131 AND\n"
    "1 1 46 129 INV\n"
    "1 1 45 128 INV\n"
    "2 1 129 128 130 AND\n"
    "2 1 131 130 139 AND\n"
    "1 1 35 133 INV\n"
    "1 1 32 132 INV\n"
    "2 1 133 132 137 AND\n"
    "1 1 34 135 INV\n"
    "1 1 33 134 INV\n"
    "2 1 135 134 136 AND\n"
    "2 1 137 136 138 AND\n"
    "2 1 139 138 155 AND\n"
    "1 1 43 141 INV\n"
    "1 1 40 140 INV\n"
    "2 1 141 140 145 AND\n"
    "1 1 42 143 INV\n"
    "1 1 41 142 INV\n"
    "2 1 143 142 144 AND\n"
    "2 1 145 144 153 AND\n"
    "1 1 39 147 INV\n"
    "1 1 36 146 INV\n"
    "2 1 147 146 151 AND\n"
    "1 1 38 149 INV\n"
    "1 1 37 148 INV\n"
    "2 1 149 148 150 AND\n"
    "2 1 151 150 152 AND\n"
    "2 1 153 152 154 AND\n"
    "2 1 155 154 187 AND\n"
    "1 1 31 157 INV\n"
    "1 1 28 156 INV\n"
    "2 1 157 156 161 AND\n"
    "1 1 30 159 INV\n"
    "1 1 29 158 INV\n"
    "2 1 159 158 160 AND\n"
    "2 1 161 160 169 AND\n"
    "1 1 19 163 INV\n"
    "1 1 16 162 INV\n"
    "2 1 163 162 167 AND\n"
    "1 1 18 165 INV\n"
    "1 1 17 164 INV\n"
    "2 1 165 164 166 AND\n"
    "2 1 167 166 168 AND\n"
    "2 1 169 168 185 AND\n"
    "1 1 27 171 INV\n"
    "1 1 24 170 INV\n"
    "2 1 171 170 175 AND\n"
    "1 1 26 173 INV\n"
    "1 1 25 172 INV\n"
    "2 1 173 172 174 AND\n"
    "2 1 175 174 183 AND\n"
    "1 1 23 177 INV\n"
    "1 1 20 176 INV\n"
    "2 1 177 176 181 AND\n"
    "1 1 22 179 INV\n"
    "1 1 21 178 INV\n"
    "2 1 179 178 180 AND\n"
    "2 1 181 180 182 AND\n"
    "2 1 183 182 184 AND\n"
    "2 1 185 184 186 AND\n"
    "2 1 187 186 188 AND\n"
    "2 1 189 188 190 AND\n"
  };

  std::stringstream ss; ss << zero_equal;

  bristol_test_reader reader;
  auto result{lorina::read_bristol( ss, reader )};
  CHECK( result == lorina::return_code::success );
}
