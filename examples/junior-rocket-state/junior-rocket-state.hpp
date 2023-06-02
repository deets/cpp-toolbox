// (c) Diez Roggisch, 2023
// SPDX-License-Identifier: MIT
#pragma once

#include <timed-finite-automaton.hpp>
#include <cstdint>
#include <ostream>

namespace far {
namespace junior {

enum class state
{
  // Start state
  IDLE,
  // After stable pressure is detected
  WAIT_FOR_LAUNCH,
  // We measured acceleration beyond the threshold,
  // but this needs to be stable for a while
  // to qualify as ACCELERATING
  ACCELERATION_DETECTED,
  // We reached the acceleration phase,
  ACCELERATING,
  // once pressure fell off enough, we declare us being
  // launched.
  LAUNCHED,
  // The motor cut out
  BURNOUT,
  // after a second, separate
  SEPARATION,
  // shortly after the separation, we are in coasting
  COASTING,
  // We detected a pressure drop, and thus reached
  // the peak.
  PEAK_REACHED,
  FALLING,
  DROUGE_OPENED,
  LANDED,
};

// Time is  measured in microseconds!
enum timeouts : uint32_t {
  ACCELERATION = 1000*1000,
  SEPARATION_TIMEOUT = 1000*1000,
};

enum class event {
  GROUND_PRESSURE_ESTABLISHED,
  PRESSURE_BELOW_LAUNCH_THRESHOLD,
  PRESSURE_ABOVE_LAUNCH_THRESHOLD,
  PRESSURE_PEAK_REACHED,
  ACCELERATION_BELOW_THRESHOLD,
  ACCELERATION_ABOVE_THRESHOLD,
  ACCELERATION_NEGATIVE,
  EXPECTED_FLIGHT_TIME_REACHED,
};

class JuniorRocketState {
  using state_machine_t = tfa::TimedFiniteAutomaton<state, event, uint32_t>;
public:
  JuniorRocketState();
  JuniorRocketState(const JuniorRocketState&) = delete;
  JuniorRocketState& operator=(const JuniorRocketState&) = delete;
  JuniorRocketState(JuniorRocketState&&) = delete;

  void dot(std::ostream& os);
private:
  state_machine_t _state_machine;
};

// To allow grahpniz output
std::ostream& operator<<(std::ostream&, const state&);
std::ostream& operator<<(std::ostream&, const event&);

} // namespace junior
} // namespace far
