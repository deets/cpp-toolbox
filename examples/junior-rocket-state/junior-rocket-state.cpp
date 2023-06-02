// (c) Diez Roggisch, 2023
// SPDX-License-Identifier: MIT


#include "junior-rocket-state.hpp"

#include <iostream>


using namespace far::junior;

JuniorRocketState::JuniorRocketState()
  : _state_machine(state::IDLE)
{
  auto& sm = _state_machine; // Just a convenient alias
  sm.add_transition(state::IDLE, event::GROUND_PRESSURE_ESTABLISHED, state::WAIT_FOR_LAUNCH);
  sm.add_transition(state::WAIT_FOR_LAUNCH, event::ACCELERATION_ABOVE_THRESHOLD, state::ACCELERATION_DETECTED);
  sm.add_transition(state::ACCELERATION_DETECTED, event::ACCELERATION_BELOW_THRESHOLD, state::WAIT_FOR_LAUNCH);
  sm.add_transition(state::ACCELERATION_DETECTED, timeouts::ACCELERATION, state::ACCELERATING);
  sm.add_transition(state::ACCELERATING, event::LAUNCH_PRESSURE_DROP, state::LAUNCHED);
}

void JuniorRocketState::dot(std::ostream &os)
{
  _state_machine.dot(os, "us");
}

namespace far::junior {
std::ostream& operator<<(std::ostream& os, const state& state)
{
  switch(state)
  {
  case state::ACCELERATION_DETECTED:
    os << "ACCELERATION_DETECTED";
    break;
  case state::IDLE:
    os << "IDLE";
    break;
  case state::LAUNCHED:
    os << "LAUNCHED";
    break;
  case state::WAIT_FOR_LAUNCH:
    os << "WAIT_FOR_LAUNCH";
    break;
  case state::ACCELERATING:
    os << "ACCELERATING";
    break;
  case state::COASTING:
    os << "COASTING";
    break;
  case state::PEAK_REACHED:
    os << "PEAK_REACHED";
    break;
  case state::FALLING:
    os << "FALLING";
    break;
  case state::DROUGE_OPENED:
    os << "DROUGE_OPENED";
    break;
  case state::LANDED:
    os << "LANDED";
    break;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const event& event)
{
  switch(event)
  {
  case event::GROUND_PRESSURE_ESTABLISHED:
    os << "GROUND_PRESSURE_ESTABLISHED";
    break;
  case event::LAUNCH_PRESSURE_DROP:
    os << "LAUNCH_PRESSURE_DROP";
    break;
  case event::ACCELERATION_ABOVE_THRESHOLD:
    os << "ACCELERATION_ABOVE_THRESHOLD";
    break;
  case event::ACCELERATION_BELOW_THRESHOLD:
    os << "ACCELERATION_BELOW_THRESHOLD";
    break;
  case event::EXPECTED_FLIGHT_TIME_REACHED:
    os << "EXPECTED_FLIGHT_TIME_REACHED";
    break;
  }
  return os;
}

} // namespace far::junior


int main(int argc, char *argv[])
{
  JuniorRocketState state;
  state.dot(std::cout);
  return 0;
}
