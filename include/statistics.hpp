// (c) Diez Roggisch, 2023
// SPDX-License-Identifier: MIT
#pragma once
#include <optional>
#include <iostream>
#include <numeric>


namespace deets::statistics {


template <typename F>
struct statistics_t
{
  F average;
  F variance;

  F stddev() const
  {
    return sqrt(variance);
  }

  bool operator==(const statistics_t<F>& other)
  {
    return average == other && variance == other;
  }

};

template <typename F, int N, int Confidence=N>
struct RollingStatistics
{
  static constexpr F n = F(N);
  using result_t = statistics_t<F>;

  RollingStatistics(F average_, F variance_)
    : average(average_)
    , variance(variance_)
    , previous(average_)
  {
  }

  F average;
  F variance;
  F previous;
  size_t updates = 0;


  std::optional<result_t> update(F value)
  {
    std::optional<result_t> result;
    ++updates;
    const auto oldavg = average;
    const auto newavg = oldavg + (value - previous) / n;
    average = newavg;
    variance += (value - previous) * (value - newavg + previous - oldavg) / (n - 1.0);
    // We  only report back if we've done this long enough
    if(updates >= Confidence)
    {
      result = { average, variance };
    }
    previous = value;
    return result;
  }
};

template<typename F, int N>
struct ArrayStatistics
{
  using result_t = statistics_t<F>;
  static constexpr F n = F(N);

  std::array<F, N> values;
  size_t updates = 0;

  std::optional<result_t> update(F value)
  {
    values[updates++ % N] = value;
    if(updates >= N)
    {
      const auto average = std::reduce(
        values.begin(), values.end()
        ) / n;
      const F variance = std::reduce(
        values.begin(), values.end(),
        0.0, [average](const F& previous, const F& current)
        {
          return previous + pow(average - current, 2);
        }
        ) / (n - 1); // Not sure exactly why, but that's the python version
      return result_t{ average, variance };
    }
    return std::nullopt;
  }

  std::optional<F> median()
  {
    if(updates >= N)
    {
      std::sort(values.begin(), values.end());
      return values[N / 2];
    }
    return std::nullopt;
  }
};

} // namespace deets::statistics
