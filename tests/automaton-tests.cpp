#include "timed-finite-automaton.hpp"

#include <catch2/catch_test_macros.hpp>
#include <sstream>

enum State { A, B, C };

enum Event {
  FOO,
  BAR,
};

using TestAutomaton = tfa::TimedFiniteAutomaton<State, Event, uint32_t>;

std::ostream &operator<<(std::ostream &os, const State &state)
{
  switch(state) {
  case A:
    os << "A";
    break;
  case B:
    os << "B";
    break;
  case C:
    os << "C";
    break;
  }
  return os;
}


std::ostream &operator<<(std::ostream &os, const Event &event)
{
  switch(event) {
  case FOO:
    os << "FOO";
    break;
  case BAR:
    os << "BAR";
    break;
  }
  return os;
}


TEST_CASE( "Instantiate Test Automaton", "[tfa]" ) {
  TestAutomaton t{A};
  REQUIRE( t.state() == A );
}


TEST_CASE( "Add state transition on event", "[tfa]" ) {
  TestAutomaton t{A};
  t.add_transition(A, FOO, B);
  REQUIRE( t.state() == A );

  SECTION("Feeding FOO transitions to B")
  {
    REQUIRE(t.feed(FOO) == true);
    REQUIRE(t.state() == B);
  }

    SECTION("Feeding BAR has no effect")
  {
    REQUIRE(t.feed(BAR) == false);
    REQUIRE(t.state() == A);
  }
}

TEST_CASE( "Add state transition on timeout", "[tfa]" ) {
  TestAutomaton t{A};
  t.add_transition(A, 1000, B);
  REQUIRE( t.state() == A );

  SECTION("Feeding elapsed below threshold doesn't do anything")
  {
    REQUIRE(t.elapsed(500) == false);
    REQUIRE(t.state() == A);
  }

  SECTION("Feeding equal to threshold triggers transition")
  {
    REQUIRE(t.elapsed(1000) == true);
    REQUIRE(t.state() == B);
  }

  SECTION("Feeding above threshold triggers transition")
  {
    REQUIRE(t.elapsed(1100) == true);
    REQUIRE(t.state() == B);
  }

  SECTION("Feeding mulitple increments eventually transitions")
  {
    for(auto i=1; i <= 10; ++i)
    {
      t.elapsed(100); // 10 * 100 == 1000
    }
    REQUIRE(t.state() == B);
  }

  SECTION("Changing state by event resets the state change timestamp")
  {
    // By adding this transition, and triggering it after the
    // time based one, we expect the time based one not to
    // work
    t.add_transition(B, FOO, A);
    REQUIRE(t.elapsed(1000) == true);
    REQUIRE(t.state() == B);
    // We progress time so that our state change
    // to the current one is beyond the duration
    // threshold and would trigger.
    REQUIRE(t.elapsed(1000) == false);

    REQUIRE(t.feed(FOO) == true);
    REQUIRE(t.state() == A);
    // Now the timer needs to be reset!
    REQUIRE(t.elapsed(100) == false);
    REQUIRE(t.state() == A);
  }

}
#ifdef USE_IOSTREAM
TEST_CASE( "graphviz rendering", "[tfa]" ) {
  TestAutomaton t{A};
  SECTION("Single state is start state")
  {
    std::stringstream ss;
    t.dot(ss, "us");
    const auto dot = ss.str();
    const char* expected = "digraph timed_finite_automaton {\n"
    "node [shape = doublecircle];\n"
    "node [style = filled];\n"
    "A;\n"
    "node [shape = circle, style = \"\"];\n"
    "}\n";
    REQUIRE(dot == expected);
  }
  SECTION("Timed transition")
  {
    std::stringstream ss;
    t.add_transition(A, 1000, B);
    t.dot(ss, "us");
    const auto dot = ss.str();
    const char* expected = "digraph timed_finite_automaton {\n"
    "node [shape = doublecircle];\n"
    "node [style = filled];\n"
    "A;\n"
    "node [shape = circle, style = \"\"];\n"
    "A->B[label = \"1000us\"];\n"
    "}\n";
    REQUIRE(dot == expected);
  }

  SECTION("Event transition")
  {
    std::stringstream ss;
    t.add_transition(A, FOO, B);
    t.dot(ss, "us");
    const auto dot = ss.str();
    const char* expected = "digraph timed_finite_automaton {\n"
    "node [shape = doublecircle];\n"
    "node [style = filled];\n"
    "A;\n"
    "node [shape = circle, style = \"\"];\n"
    "A->B[label = \"FOO\"];\n"
    "}\n";

    REQUIRE(dot == expected);
  }

  SECTION("Event and timeout transition")
  {
    std::stringstream ss;
    t.add_transition(A, FOO, B);
    t.add_transition(A, 1000, B);
    t.dot(ss, "us");
    const auto dot = ss.str();
    const char* expected =
    "digraph timed_finite_automaton {\n"
    "node [shape = doublecircle];\n"
    "node [style = filled];\n"
    "A;\n"
    "node [shape = circle, style = \"\"];\n"
    "A->B[label = \"1000us\"];\n"
    "A->B[label = \"FOO\"];\n"
    "}\n";
    REQUIRE(dot == expected);
  }

  SECTION("Active state is marked")
  {
    std::stringstream ss;
    t.add_transition(A, FOO, B);
    t.feed(FOO);
    t.dot(ss, "us");
    const auto dot = ss.str();
    const char* expected =
    "digraph timed_finite_automaton {\n"
    "node [shape = doublecircle];\n"
    "A;\n"
    "node [shape = circle, style = filled];\n"
    "B;\n"
    "node [shape = circle, style = \"\"];\n"
    "A->B[label = \"FOO\"];\n"
    "}\n";
    REQUIRE(dot == expected);
  }
}
#endif
