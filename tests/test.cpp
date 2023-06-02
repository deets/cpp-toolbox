#include <catch2/catch_test_macros.hpp>

#include "timed-finite-automaton.hpp"

enum State { A, B, C };

enum Event {
  FOO,
  BAR,
};

using TestAutomaton = tfa::TimedFiniteAutomaton<State, Event, uint32_t>;

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

  SECTION("Feeding above threshold triggers transition")
  {
    REQUIRE(t.elapsed(1000) == true);
    REQUIRE(t.state() == B);
  }
}
