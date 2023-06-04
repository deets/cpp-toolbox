// (c) Diez Roggisch, 2023
// SPDX-License-Identifier: MIT
#pragma once
#include <optional>
#include <iostream>

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

} // namespace deets::statistics
