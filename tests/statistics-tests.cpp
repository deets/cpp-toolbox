#include "statistics.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <sstream>

using namespace deets::statistics;

TEST_CASE( "Rolling Statistics", "[statistics]" ) {
  using test_t = RollingStatistics<double, 10>;
  test_t stats{10.0, 1.0};

  SECTION("First update returns nullopt")
  {
    REQUIRE(stats.update(10.0) == std::nullopt);
  }

  SECTION("Only after confidence is reached, we get values")
  {
    REQUIRE(stats.update(10.0) == std::nullopt);
    REQUIRE(stats.update(10.0) == std::nullopt);
    REQUIRE(stats.update(10.0) == std::nullopt);
    REQUIRE(stats.update(10.0) == std::nullopt);
    REQUIRE(stats.update(10.0) == std::nullopt);
    REQUIRE(stats.update(10.0) == std::nullopt);
    REQUIRE(stats.update(10.0) == std::nullopt);
    REQUIRE(stats.update(10.0) == std::nullopt);
    REQUIRE(stats.update(10.0) == std::nullopt);
    const auto result = *stats.update(10.0);
    REQUIRE(result.average == 10.0);
  }
}

TEST_CASE( "Rolling Statistics with explicit confidence", "[statistics]" ) {
  using test_t = RollingStatistics<double, 10, 3>;
  test_t stats{10.0, 1.0};

  SECTION("Only after confidence is reached, we get values")
  {
    REQUIRE(stats.update(10.0) == std::nullopt);
    REQUIRE(stats.update(10.0) == std::nullopt);
    const auto result = *stats.update(10.0);
    REQUIRE(result.average == 10.0);
  }
}

TEST_CASE("Array statistics", "[statistics]")
{
  using test_t = ArrayStatistics<double, 10>;
  test_t stats;

  SECTION("Only return vallues after array has been filled once")
  {
    for(auto i=0; i < 9; ++i)
    {
      REQUIRE(stats.update(10.0) == std::nullopt);
    }
    REQUIRE(stats.update(10.0) != std::nullopt);
  }

  SECTION("An actual mean is computed")
  {
    decltype(stats.update(0.0)) result = std::nullopt;
    for(auto i=1; i < 11; ++i)
    {
      result = stats.update(i);
    }
    REQUIRE(result->average == 5.5);
  }

  SECTION("An actual variance is computed")
  {
    decltype(stats.update(0.0)) result = std::nullopt;
    for(auto i=1; i < 11; ++i)
    {
      result = stats.update(i);
    }
    REQUIRE(result->variance == 9.166666666666666);
  }

  SECTION("Median works")
  {
    decltype(stats.update(0.0)) result = std::nullopt;
    for(auto i=1; i < 11; ++i)
    {
      stats.update(i);
    }
    REQUIRE(*stats.median() == 6);
  }
}

TEST_CASE("Array statistics with float", "[statistics]")
{
  using test_t = ArrayStatistics<float, 10>;
  test_t stats;
  decltype(stats.update(0.0)) result = std::nullopt;
  for(auto i=1; i < 11; ++i)
  {
    result = stats.update(i);
  }
  REQUIRE(result->variance == Catch::Approx(9.16667f));
}

// #include <iostream>
// #include <Eigen/Dense>

// using namespace Eigen;

// void fitParabola(const VectorXd& x, const VectorXd& y)
// {
//     // Check if the sizes of x and y match
//     if (x.size() != y.size())
//     {
//         std::cout << "Error: The sizes of x and y do not match." << std::endl;
//         return;
//     }

//     // Build the Vandermonde matrix
//     int n = x.size();
//     MatrixXd A(n, 3);
//     for (int i = 0; i < n; i++)
//     {
//         A(i, 0) = x(i) * x(i);
//         A(i, 1) = x(i);
//         A(i, 2) = 1.0;
//     }

//     // Solve the least squares problem
//     Vector3d coefficients = A.colPivHouseholderQr().solve(y);

//     // Display the fitted parabola equation
//     std::cout << "Fitted Parabola: y = " << coefficients(0) << " * x^2 + "
//               << coefficients(1) << " * x + " << coefficients(2) << std::endl;
// }


TEST_CASE("Parabola fitting")
{
}
