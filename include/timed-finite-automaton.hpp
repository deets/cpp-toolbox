// (c) Diez Roggisch, 2023
// SPDX-License-Identifier: MIT
#pragma once

#include <variant>
#include <unordered_map>


namespace tfa {

template<typename State, typename Event, typename TimePoint>
class TimedFiniteAutomaton {
public:
  using Duration = decltype(TimePoint{} - TimePoint{});
  using Transition = std::variant<Event, Duration>;

  TimedFiniteAutomaton(State start_state) : _state{start_state} {}

  State state() { return _state; }

  void add_transition(State from, Event what, State to)
  {
    _transitions[from][Transition{what}] = to;
  }

  bool feed(Event what)
  {
    if(_transitions.count(_state))
    {
      auto& candidate = _transitions[_state];
      if(candidate.count(what))
      {
        _state = candidate[what];
        return true;
      }
    }
    return false;
  }

private:
  State _state;
  std::unordered_map<State, std::unordered_map<Transition, State>> _transitions;

};

} // namespace tfa
