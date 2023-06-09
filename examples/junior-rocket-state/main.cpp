#include "junior-rocket-state.hpp"
#include "simulator.hpp"
#include <chrono>
#include <iostream>

using namespace far::junior;

namespace {

using namespace std::chrono_literals;

class PrintObserver : public StateObserver
{
  void data(timestamp_t timestamp, float pressure, float acceleration) override
  {
    std::cout << "data: " << us_since_start(timestamp) << ", p: " << pressure << ", a: " << acceleration << "\n";
  };
  void state_changed(timestamp_t timestamp, state to) override
  {
    std::cout << "state changed: " << us_since_start(timestamp) << " -> " << to << "\n";
    elasped_since_state_changed = duration_t::zero();
  };
  virtual void event_produced(timestamp_t timestamp, event e) override
  {
    std::cout << "event fed: " << us_since_start(timestamp) << " -> " << e << "\n";
  };
  void elapsed(timestamp_t timestamp, duration_t elapsed) override
  {
    elasped_since_state_changed += elapsed;
    std::cout << "elapsed: " << us_since_start(timestamp) << ": " << elasped_since_state_changed / 1ms << "\n";
  }

  uint32_t us_since_start(timestamp_t t)
  {
    using namespace std::chrono_literals;
    return (t - start) / 1us;
  }
  timestamp_t start = std::chrono::steady_clock::now();
  duration_t elasped_since_state_changed = duration_t::zero();

};

// [](state from, state to, uint32_t timestamp) {
//   const float at = float(timestamp) / 1000 * 1000;
//

}

int main(int argc, char *argv[])
{
  if(argc == 2 && std::string(argv[1]) == "dot")
  {
    StateObserver nop;
    JuniorRocketState state_machine(nop);
    state_machine.dot(std::cout);
  }
  else if(argc == 4 && std::string(argv[1]) == "csv")
  {
    PrintObserver printer;
    JuniorRocketState state_machine(printer);
    load_and_drive(argv[2], argv[3], state_machine);
  }
  else
  {
    std::cerr << "Unknown command\r\n";
    return 1;
  }
  return 0;
}
