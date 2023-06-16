#include <catch2/catch_test_macros.hpp>

#include <Eigen/Eigen>

#include <vector>
#include <iostream>

namespace {
std::vector<double> linspace(double start, double end, int steps)
{
  std::vector<double> result;
  const auto step = (end - start) / (steps - 1);
  for(int i=0; i < steps; ++i)
  {
    result.push_back(start + i * step);
  }
  return result;
}

} // namespace


TEST_CASE("parabola", "[fitting]" ) {
  constexpr float a = 0.7481124787152704;
  constexpr float b = -9.132908263690277;
  constexpr float c = 1004.4882956528269;
  const auto xs = linspace(5.0, 10.0, 50);
  std::vector<double> ys;
  for(const auto x : xs)
  {
    ys.push_back(pow(x, 2) * a + x * b + c);
  }
  Eigen::MatrixXd M = Eigen::MatrixXd::Ones(xs.size(), 3);
  Eigen::VectorXd beta = Eigen::VectorXd::Zero(xs.size());
  auto row = 0;
  for(const auto x : xs)
  {
    // Third column is ones already
    M(row, 0) = pow(x, 2);
    M(row, 1) = x;
    beta[row] = ys[row];
    ++row;
  }
  Eigen::VectorXd solution = (M.transpose() * M).ldlt().solve(M.transpose() * beta);
  std::cout << "The solution using normal equations is:\n" <<  solution << "\n" << solution.rows() << ", " << solution.cols() << "\n";
  std::cout << "Beta: " << beta.rows() << ", " << beta.cols() << "\n";
  Eigen::VectorXd error = M * solution;// - beta;
  error -= beta;
  std::cout << "Error: " << error.rows() << ", " << error.cols() << "\n";

  const double e = error.dot(error);
  std::cout << "The error is:\n" <<  e << "\n";
}
