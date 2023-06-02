// (c) Diez Roggisch, 2023
// SPDX-License-Identifier: MIT
#pragma once

#include <unordered_map>


namespace tfa {

template<typename State, typename Event, typename TimePoint>
class TimedFiniteAutomaton {
public:
  using Duration = decltype(TimePoint{} - TimePoint{});

  TimedFiniteAutomaton(State start_state) : _state{start_state} {}

  State state() { return _state; }

  void add_transition(State from, Event what, State to)
  {
    _event_transitions[from][what] = to;
  }

  void add_transition(State from, Duration after, State to)
  {
    _timeout_transitions[from] = std::make_pair(after, to);
  }

  bool elapsed(Duration duration)
  {
    if(_timeout_transitions.count(_state))
    {
      auto[timeout, to] = _timeout_transitions[_state];
      if(duration >= timeout)
      {
        _state = to;
        return true;
      }
    }
    return false;
  }

  bool feed(Event what)
  {
    if(_event_transitions.count(_state))
    {
      auto& candidate = _event_transitions[_state];
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
  std::unordered_map<State, std::unordered_map<Event, State>> _event_transitions;
  std::unordered_map<State, std::pair<Duration, State>> _timeout_transitions;
};

} // namespace tfa
