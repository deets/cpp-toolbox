// (c) Diez Roggisch, 2023
// SPDX-License-Identifier: MIT
#pragma once

#include <timed-finite-automaton.hpp>
#include <cstdint>
#include <ostream>
#include <optional>

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
  MEASURE_FALLING_PRESSURE1,
  MEASURE_FALLING_PRESSURE2,
  MEASURE_FALLING_PRESSURE3,
  DROUGE_OPENED,
  DROUGE_FAILED,
  LANDED,
};

// Time is  measured in microseconds!
enum timeouts : uint32_t {
  ACCELERATION = 400*1000,
  SEPARATION_TIMEOUT = 1000*1000,
  MOTOR_BURNTIME = 2500*1000,
  FALLING_PRESSURE_TIMEOUT = 1000*1000,
};

// We seem to reach a shoulder of ~20m/s^2, so
// this looks safe
constexpr float LAUNCH_ACCELERATION_THRESHOLD = 15.0;
// We only get an absolute acceleration, so it is never
// below zero, instead we need to
constexpr float FREEFALL_ACCELERATION_THRESHOLD = 3.0;
// mbar
constexpr float LAUNCH_PRESSURE_DIFFERENTIAL = 5.0;

enum class event {
  GROUND_PRESSURE_ESTABLISHED,
  // Happens when the difference between ground pressure
  // and measured pressure is bigger than LAUNCH_PRESSURE_DIFFERENTIAL
  // which means that we are higher in the air. The name is thus
  // inverted
  PRESSURE_BELOW_LAUNCH_THRESHOLD,
  PRESSURE_ABOVE_LAUNCH_THRESHOLD,
  PRESSURE_PEAK_REACHED,
  ACCELERATION_BELOW_THRESHOLD,
  ACCELERATION_ABOVE_THRESHOLD,
  ACCELERATION_AROUND_ZERO,
  EXPECTED_APOGEE_TIME_REACHED,
  PRESSURE_LINEAR,
  PRESSURE_QUADRATIC,
  RESTART_PRESSURE_MEASUREMENT,
};


struct StateObserver {
  virtual void data(uint32_t timestamp, float pressure, float acceleration) {};
  virtual void state_changed(uint32_t timestamp, state) {};
  virtual void event_produced(uint32_t timestamp, event) {};
  virtual void elapsed(uint32_t timestamp, uint32_t elapsed) {};
};

class JuniorRocketState {
  using state_machine_t = tfa::TimedFiniteAutomaton<state, event, uint32_t>;

public:

  JuniorRocketState(StateObserver&);
  JuniorRocketState(const JuniorRocketState&) = delete;
  JuniorRocketState& operator=(const JuniorRocketState&) = delete;
  JuniorRocketState(JuniorRocketState&&) = delete;

  void dot(std::ostream& os);
  void drive(uint32_t, float, float);
  std::optional<uint32_t> flighttime() const;

private:
  void process_pressure(float pressure);
  void produce_events(uint32_t timestamp, float pressure, float acceleration);
  void handle_state_transition(state to);
  void feed(uint32_t timestamp, event);

  state_machine_t _state_machine;

  std::optional<uint32_t> _last_timestamp;
  std::optional<float> _ground_pressure;
  std::optional<uint32_t> _liftoff_timestamp;

  StateObserver& _state_observer;

};

// To allow grahpniz output
std::ostream& operator<<(std::ostream&, const state&);
std::ostream& operator<<(std::ostream&, const event&);

} // namespace junior
} // namespace far
