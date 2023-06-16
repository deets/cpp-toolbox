#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <Eigen/Eigen>

#include <vector>
#include <iostream>
#include <optional>


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

template <size_t Samples, typename Scalar=double>
class ParabolaClassifier
{
public:
  using coefficients_t = Eigen::Vector<Scalar, 3>;

  bool feed(Scalar x, Scalar y)
  {
    _xs[insertions % Samples] = x;
    _ys[insertions++ % Samples] = y;
    if(insertions >= Samples)
    {
      solve();
      return true;
    }
    return false;
  }

  std::optional<coefficients_t> coefficients() const
  {
    return _coefficients;
  }

private:
  void solve()
  {
    auto row = 0;
    for(const auto x : _xs)
    {
      _M(row, 0) = pow(x, 2);
      _M(row, 1) = x;
      _M(row, 2) = 1.0;
      _b[row] = _ys[row];
      ++row;
    }
    _coefficients = (_M.transpose() * _M).ldlt().solve(_M.transpose() * _b);
  }

  std::array<Scalar, Samples> _xs;
  std::array<Scalar, Samples> _ys;
  size_t insertions = 0;

  Eigen::Matrix<Scalar, Samples, 3> _M;
  Eigen::Vector<Scalar, Samples> _b;
  std::optional<coefficients_t> _coefficients;

};

} // namespace


TEST_CASE("parabola", "[fitting]" )
{
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

TEST_CASE("parabola class", "[fitting]" )
{
  constexpr float a = 0.7481124787152704;
  constexpr float b = -9.132908263690277;
  constexpr float c = 1004.4882956528269;
  constexpr size_t samples = 50;
  ParabolaClassifier<samples> classifier;

  const auto xs = linspace(5.0, 10.0, samples);
  std::vector<double> ys;
  for(const auto x : xs)
  {
    ys.push_back(pow(x, 2) * a + x * b + c);
  }

  SECTION("feeding enough values makes classification valid")
  {
    for(auto i=0; i < (samples - 1); ++i)
    {
      REQUIRE(classifier.feed(0.0, 0.0) == false);
    }
    REQUIRE(classifier.feed(0.0, 0.0));
  }

  SECTION("fitting results in matching coefficients")
  {
    for(auto i=0; i < samples; ++i)
    {
      classifier.feed(xs[i], ys[i]);
    }
    const auto coeffs = *classifier.coefficients();
    REQUIRE(coeffs[0] == Catch::Approx(a));
    REQUIRE(coeffs[1] == Catch::Approx(b));
    REQUIRE(coeffs[2] == Catch::Approx(c));
  }
}
