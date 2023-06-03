#include "junior-rocket-state.hpp"
#include "simulator.hpp"
#include <iostream>

using namespace far::junior;

namespace {

class PrintObserver : public StateObserver
{
  void data(uint32_t timestamp, float pressure, float acceleration) override
  {
    std::cout << "data: " << timestamp << ", p: " << pressure << ", a: " << acceleration << "\n";
  };
  void state_changed(uint32_t timestamp, state to) override
  {
    std::cout << "state changed: " << timestamp << " -> " << to << "\n";
    elasped_since_state_changed = 0;
  };
  virtual void event_produced(uint32_t timestamp, event e) override
  {
    std::cout << "event fed: " << timestamp << " -> " << e << "\n";
  };
  void elapsed(uint32_t timestamp, uint32_t elapsed) override
  {
    elasped_since_state_changed += elapsed;
    std::cout << "elapsed: " << timestamp << ": " << elasped_since_state_changed << "\n";
  }
  uint32_t elasped_since_state_changed = 0;

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
