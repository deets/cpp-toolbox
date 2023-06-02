// (c) Diez Roggisch, 2023
// SPDX-License-Identifier: MIT
#pragma once
#include <ostream>
#include <unordered_map>

namespace tfa {

template<typename State, typename Event, typename TimePoint>
class TimedFiniteAutomaton {
public:
  using Duration = decltype(TimePoint{} - TimePoint{});

  TimedFiniteAutomaton(State start_state)
    : _state{start_state}
    , _start_state{start_state}
    , _state_change{}
    , _now{}
  {}

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
    _now += duration;
    const auto elapsed = _now - _state_change;

    if(_timeout_transitions.count(_state))
    {
      auto[timeout, to] = _timeout_transitions[_state];
      if(elapsed >= timeout)
      {
        _state = to;
        _state_change = _now;
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
        _state_change = _now;
        return true;
      }
    }
    return false;
  }

  void dot(std::ostream& os) {
    os << "digraph timed_finite_automaton {\n";
    os << "node [shape = doublecircle];";
    os << _start_state << ";\n";
    os << "node [shape = circle];\n";
    for(const auto& transition : _timeout_transitions)
    {
      const auto [from, edge] = transition;
      const auto [timeout, to] = edge;
      os << from << "->" << to << "[label = \"" << timeout << "\"];\n";
    }
    os << "}\n";
  }

private:
  State _start_state, _state;
  TimePoint _state_change;
  TimePoint _now;

  std::unordered_map<State, std::unordered_map<Event, State>> _event_transitions;
  std::unordered_map<State, std::pair<Duration, State>> _timeout_transitions;
};

} // namespace tfa
