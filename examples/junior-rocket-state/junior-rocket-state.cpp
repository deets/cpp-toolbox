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
  sm.add_transition(state::ACCELERATING, event::ACCELERATION_BELOW_THRESHOLD, state::WAIT_FOR_LAUNCH);
  sm.add_transition(state::ACCELERATING, event::PRESSURE_BELOW_LAUNCH_THRESHOLD, state::LAUNCHED);
  sm.add_transition(state::LAUNCHED, event::ACCELERATION_NEGATIVE, state::BURNOUT);
  sm.add_transition(state::LAUNCHED, timeouts::MOTOR_BURNTIME - timeouts::ACCELERATION, state::BURNOUT);
  sm.add_transition(state::BURNOUT, timeouts::SEPARATION_TIMEOUT, state::SEPARATION);
  sm.add_transition(state::SEPARATION, 0, state::COASTING);
  sm.add_transition(state::COASTING, event::PRESSURE_PEAK_REACHED, state::FALLING);
  sm.add_transition(state::COASTING, event::EXPECTED_APOGEE_TIME_REACHED, state::FALLING);
  sm.add_transition(state::FALLING, timeouts::FALLING_PRESSURE_TIMEOUT, state::MEASURE_FALLING_PRESSURE1);
  sm.add_transition(state::MEASURE_FALLING_PRESSURE1, timeouts::FALLING_PRESSURE_TIMEOUT, state::MEASURE_FALLING_PRESSURE2);
  sm.add_transition(state::MEASURE_FALLING_PRESSURE2, timeouts::FALLING_PRESSURE_TIMEOUT, state::MEASURE_FALLING_PRESSURE3);
  sm.add_transition(state::MEASURE_FALLING_PRESSURE3, event::PRESSURE_LINEAR, state::DROUGE_OPENED);
  sm.add_transition(state::MEASURE_FALLING_PRESSURE3, event::PRESSURE_QUADRATIC, state::DROUGE_FAILED);
  sm.add_transition(state::DROUGE_OPENED, event::PRESSURE_BELOW_LAUNCH_THRESHOLD, state::LANDED);
  sm.add_transition(state::DROUGE_FAILED, event::PRESSURE_BELOW_LAUNCH_THRESHOLD, state::LANDED);
  sm.add_transition(state::DROUGE_FAILED, event::RESTART_PRESSURE_MEASUREMENT, state::FALLING);
}

void JuniorRocketState::dot(std::ostream &os)
{
  _state_machine.dot(os, "us");
}

namespace far::junior {

#define M_STATE(_state) \
  case state::_state: \
  os << #_state; \
  break;


std::ostream& operator<<(std::ostream& os, const state& state)
{
  switch(state)
  {
    M_STATE(IDLE)
    M_STATE(WAIT_FOR_LAUNCH)
    M_STATE(ACCELERATION_DETECTED)
    M_STATE(ACCELERATING)
    M_STATE(LAUNCHED)
    M_STATE(BURNOUT)
    M_STATE(SEPARATION)
    M_STATE(COASTING)
    M_STATE(PEAK_REACHED)
    M_STATE(FALLING)
    M_STATE(MEASURE_FALLING_PRESSURE1)
    M_STATE(MEASURE_FALLING_PRESSURE2)
    M_STATE(MEASURE_FALLING_PRESSURE3)
    M_STATE(DROUGE_OPENED)
    M_STATE(DROUGE_FAILED)
    M_STATE(LANDED);
  }
  return os;
}

#define M_EVENT(_event) \
  case event::_event: \
  os << #_event; \
  break;

std::ostream& operator<<(std::ostream& os, const event& event)
{
  switch(event)
  {
    M_EVENT(GROUND_PRESSURE_ESTABLISHED)
    M_EVENT(PRESSURE_BELOW_LAUNCH_THRESHOLD)
    M_EVENT(PRESSURE_ABOVE_LAUNCH_THRESHOLD)
    M_EVENT(PRESSURE_PEAK_REACHED)
    M_EVENT(ACCELERATION_BELOW_THRESHOLD)
    M_EVENT(ACCELERATION_ABOVE_THRESHOLD)
    M_EVENT(ACCELERATION_NEGATIVE)
    M_EVENT(PRESSURE_LINEAR)
    M_EVENT(PRESSURE_QUADRATIC)
    M_EVENT(RESTART_PRESSURE_MEASUREMENT)
    M_EVENT(EXPECTED_APOGEE_TIME_REACHED);
  }
  return os;
}

} // namespace far::junior


int main(int argc, char *argv[])
{
  JuniorRocketState state;
  if(argc == 2 && std::string(argv[1]) == "dot")
  {
    state.dot(std::cout);
  }
  else
  {
    std::cerr << "Unknown command\r\n";
    return 1;
  }
  return 0;
}
