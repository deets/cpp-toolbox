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
  sm.add_transition(state::ACCELERATING, event::PRESSURE_BELOW_LAUNCH_THRESHOLD, state::LAUNCHED);
  sm.add_transition(state::LAUNCHED, event::ACCELERATION_NEGATIVE, state::BURNOUT);
  sm.add_transition(state::BURNOUT, timeouts::SEPARATION_TIMEOUT, state::SEPARATION);
  sm.add_transition(state::SEPARATION, 0, state::COASTING);
  sm.add_transition(state::COASTING, event::PRESSURE_PEAK_REACHED, state::FALLING);
  sm.add_transition(state::FALLING, 0, state::DROUGE_OPENED);
  sm.add_transition(state::DROUGE_OPENED, event::PRESSURE_ABOVE_LAUNCH_THRESHOLD, state::LANDED);
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
    M_STATE(DROUGE_OPENED)
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
    M_EVENT(EXPECTED_FLIGHT_TIME_REACHED);
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
