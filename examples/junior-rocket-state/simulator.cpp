#include "simulator.hpp"

#include <iostream>
#include <fstream>
#include <iterator>
#include <sstream>
#include <locale>
#include <chrono>

namespace far::junior {

using namespace std::chrono_literals;

struct data_row_t {
  timestamp_t time;
  float totalacc;
  float pressure;

};

namespace {
using namespace std::chrono_literals;

struct comma_is_space : std::ctype<char> {
  comma_is_space() : std::ctype<char>(get_table()) {}
  static mask const* get_table()
  {
    static mask rc[table_size];
    rc[size_t(',')] = std::ctype_base::space;
    rc[size_t('\n')] = std::ctype_base::space;
    return &rc[0];
  }
};

std::vector<data_row_t> load_data(const char *filename, std::chrono::steady_clock::time_point start)
{
  std::vector<data_row_t> result;
  std::ifstream inf(filename);
  assert(inf.is_open());
  for( std::string line; getline(inf, line);)
  {
    if(line.size() > 1 && line[0] != '#')
    {
      std::istringstream iss(line);
      iss.imbue(std::locale(iss.getloc(), new comma_is_space()));
      data_row_t item;
      float raw_timestamp;
      iss >> raw_timestamp >> item.totalacc >> item.pressure;
      item.time = start + (uint32_t(raw_timestamp * 1000000.0) * 1us);
      result.emplace_back(item);
    }
  }
  return result;
}

void drive(const std::vector<data_row_t>& data, JuniorRocketState &state)
{
  for(const auto& entry : data)
  {
    state.drive(entry.time, entry.pressure, entry.totalacc);
  }

}

std::vector<data_row_t>
combine_data(const std::vector<data_row_t> &first_stage_data,
             const std::vector<data_row_t> &second_stage_data)
{
  std::vector<data_row_t> result;
  const auto cutoff_time = first_stage_data[0].time;

  for(const auto& entry : second_stage_data)
  {
    if(entry.time < cutoff_time)
    {
      result.push_back(entry);
    }
  }
  std::copy(
    first_stage_data.begin(), first_stage_data.end(),
    std::back_inserter(result));
  return result;
}

}

void load_and_drive(const char* first_stage_filename, const char* second_stage_filename, JuniorRocketState& state)
{
  const auto start = std::chrono::steady_clock::now();
  auto first_stage_data = load_data(first_stage_filename, start);
  auto second_stage_data = load_data(second_stage_filename, start);
  if(first_stage_data.size() == 0 || second_stage_data.size() == 0)
  {
    std::cerr << "Couldn't load data, check file\n";
  }
  if(first_stage_data.size() > second_stage_data.size())
  {
    std::cerr << "It appears " << first_stage_filename << " contains only first stage data, swapping.\n";
    auto h = first_stage_data;
    first_stage_data = second_stage_data;
    second_stage_data = h;
  }
  const auto full_first_stage_data = combine_data(first_stage_data, second_stage_data);
  std::cerr << "Loaded " << full_first_stage_data.size() << " entries\n";
  drive(full_first_stage_data, state);
}

}
