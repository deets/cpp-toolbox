// (c) Diez Roggisch, 2023
// SPDX-License-Identifier: MIT


#include "junior-rocket-state.hpp"

#include <_types/_uint32_t.h>
#include <iostream>

using namespace far::junior;

JuniorRocketState::JuniorRocketState(StateObserver& state_observer)
  : _state_machine(state::IDLE)
  , _state_observer(state_observer)
{
  auto& sm = _state_machine; // Just a convenient alias
  sm.add_transition(state::IDLE, event::GROUND_PRESSURE_ESTABLISHED, state::WAIT_FOR_LAUNCH);
  sm.add_transition(state::WAIT_FOR_LAUNCH, event::ACCELERATION_ABOVE_THRESHOLD, state::ACCELERATION_DETECTED);
  sm.add_transition(state::ACCELERATION_DETECTED, event::ACCELERATION_BELOW_THRESHOLD, state::WAIT_FOR_LAUNCH);
  sm.add_transition(state::ACCELERATION_DETECTED, timeouts::ACCELERATION, state::ACCELERATING);
  sm.add_transition(state::ACCELERATING, event::ACCELERATION_BELOW_THRESHOLD, state::WAIT_FOR_LAUNCH);
  sm.add_transition(state::ACCELERATING, event::PRESSURE_BELOW_LAUNCH_THRESHOLD, state::LAUNCHED);
  sm.add_transition(state::LAUNCHED, event::ACCELERATION_AROUND_ZERO, state::BURNOUT);
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
  sm.add_transition(state::DROUGE_OPENED, event::PRESSURE_ABOVE_LAUNCH_THRESHOLD, state::LANDED);
  sm.add_transition(state::DROUGE_FAILED, event::PRESSURE_ABOVE_LAUNCH_THRESHOLD, state::LANDED);
  sm.add_transition(state::DROUGE_FAILED, event::RESTART_PRESSURE_MEASUREMENT, state::FALLING);
}


  // acceleration > threshold -> tentative start time of flight
  // reached launched state: start time of flight confirmed

  // RESTART_PRESSURE_MEASUREMENT can only be generated when
  // PRESSURE_BELOW_LAUNCH_THRESHOLD
  // if(acceleration > threshold)
  // {
  //   _state_machine.feed(event::ACCELERATION_ABOVE_THRESHOLD);
  // }
  // else
  // {
  //    _state_machine.feed(event::ACCELERATION_BELOW_THRESHOLD);
  //    if(acceleration < 0)
  //    {
  //      _state_machine.feed(event::ACCELERATION_NEGATIVE);
  //    }
  // }
  // if

void JuniorRocketState::process_pressure(float pressure)
{
  // TODO: this is not correct, but we don't have a proper
  // pressure pre-amble yet.
  if(!_ground_pressure)
  {
    _ground_pressure = pressure;
  }
}

void JuniorRocketState::produce_events(uint32_t timestamp, float pressure, float acceleration)
{
  if(_ground_pressure) {
    feed(timestamp, event::GROUND_PRESSURE_ESTABLISHED);
    if(*_ground_pressure - pressure >= LAUNCH_PRESSURE_DIFFERENTIAL)
    {
      feed(timestamp, event::PRESSURE_BELOW_LAUNCH_THRESHOLD);
    }
    else
    {
      feed(timestamp, event::PRESSURE_ABOVE_LAUNCH_THRESHOLD);
    }
  }

  if(acceleration > LAUNCH_ACCELERATION_THRESHOLD)
  {
    feed(timestamp, event::ACCELERATION_ABOVE_THRESHOLD);
  }
  else
  {
    feed(timestamp, event::ACCELERATION_BELOW_THRESHOLD);
    if(acceleration < FREEFALL_ACCELERATION_THRESHOLD)
    {
      feed(timestamp, event::ACCELERATION_AROUND_ZERO);
    }
  }
}

void JuniorRocketState::feed(uint32_t timestamp, event e)
{
  _state_machine.feed(e);
  _state_observer.event_produced(timestamp, e);
}

void JuniorRocketState::handle_state_transition(state to)
{
  switch(to)
  {
  case state::WAIT_FOR_LAUNCH:
    _liftoff_timestamp = std::nullopt;
    break;
  case state::ACCELERATION_DETECTED:
    _liftoff_timestamp = *_last_timestamp;
    break;
  default:
    break;
  }
}

void JuniorRocketState::drive(uint32_t timestamp, float pressure, float acceleration)
{
  _state_observer.data(timestamp, pressure, acceleration);
  if(!_last_timestamp)
  {
    _last_timestamp = timestamp;
    return;
  }
  // TODO: timediff!
  const auto elapsed = timestamp - *_last_timestamp;
  _last_timestamp = timestamp;

  const auto old = _state_machine.state();

  // Drive timer events
  _state_machine.elapsed(elapsed);
  _state_observer.elapsed(timestamp, elapsed);

  process_pressure(pressure);
  produce_events(timestamp, pressure, acceleration);

  const auto to = _state_machine.state();
  if(old != to)
  {
    handle_state_transition(to);
    _state_observer.state_changed(timestamp, to);
  }

}

std::optional<uint32_t> JuniorRocketState::flighttime() const
{
  if(_liftoff_timestamp)
  {
    // We know _last_timestamp must be valid, as
    // no liftoff could exist otherwise
    // TODO: timediff!
    return *_last_timestamp - *_liftoff_timestamp;
  }
  return std::nullopt;
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
    M_EVENT(ACCELERATION_AROUND_ZERO)
    M_EVENT(PRESSURE_LINEAR)
    M_EVENT(PRESSURE_QUADRATIC)
    M_EVENT(RESTART_PRESSURE_MEASUREMENT)
    M_EVENT(EXPECTED_APOGEE_TIME_REACHED);
  }
  return os;
}

} // namespace far::junior
