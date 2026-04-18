#include <algorithm>
#include <benchmark/benchmark.h>
#include <execution>
#include <experimental/bits/simd.h>
#include <experimental/simd>
#include <random>
#include <ranges>

#include "external.hpp"

namespace {
  void bm_vector_multiply_double(benchmark::State & state) {
    constexpr int num_elements = 1000000;
    std::mt19937_64 rng(42);// NOLINT
    std::uniform_real_distribution<> dist(0.0, 100.0);
    std::vector<double> a = std::views::iota(0, num_elements) |
                            std::views::transform([&](int) { return dist(rng); }) |
                            std::ranges::to<std::vector>();

    std::vector<double> b = std::views::iota(0, num_elements) |
                            std::views::transform([&](int) { return dist(rng); }) |
                            std::ranges::to<std::vector>();
    for (auto _ : state) {
      std::vector<double> c(num_elements);
      for (size_t i = 0; i < a.size(); ++i) {
        c[i] = a[i] * b[i];
        modify(a);
        modify(b);
      }
      benchmark::DoNotOptimize(c);
      // Perform vector multiplication here
    }
    state.counters["FLOP"] = {static_cast<double>(2),
                              benchmark::Counter::kIsIterationInvariantRate};
  }

  void bm_vector_multiply_double_unseq(benchmark::State & state) {
    constexpr int num_elements = 1000000;
    std::mt19937_64 rng(42);// NOLINT
    std::uniform_real_distribution<> dist(0.0, 100.0);
    std::vector<double> a = std::views::iota(0, num_elements) |
                            std::views::transform([&](int) { return dist(rng); }) |
                            std::ranges::to<std::vector>();

    std::vector<double> b = std::views::iota(0, num_elements) |
                            std::views::transform([&](int) { return dist(rng); }) |
                            std::ranges::to<std::vector>();
    for (auto _ : state) {
      std::vector<double> c(num_elements);
      std::transform(std::execution::unseq, a.begin(), a.end(), b.begin(), c.begin(),
                     [&](double x, double y) {
                       modify(a);
                       modify(b);
                       return x * y;
                     });
      // Perform vector multiplication here in parallel
      benchmark::DoNotOptimize(c);
    }
    state.counters["FLOP"] = {static_cast<double>(2),
                              benchmark::Counter::kIsIterationInvariantRate};
  }

  void bm_vector_multiply_double_simd(benchmark::State & state) {
    namespace stdx = std::experimental;
    constexpr int num_elements = 1000000;
    std::mt19937_64 rng(42);// NOLINT
    std::uniform_real_distribution<> dist(0.0, 100.0);
    std::vector<double> a = std::views::iota(0, num_elements) |
                            std::views::transform([&](int) { return dist(rng); }) |
                            std::ranges::to<std::vector>();

    std::vector<double> b = std::views::iota(0, num_elements) |
                            std::views::transform([&](int) { return dist(rng); }) |
                            std::ranges::to<std::vector>();
    for (auto _ : state) {
      std::vector<double> c(num_elements);
      std::size_t i = 0;
      for (; i < a.size(); i += stdx::simd<double>::size()) {
        stdx::simd<double> va(&a[i], stdx::element_aligned);
        stdx::simd<double> vb(&b[i], stdx::element_aligned);
        stdx::simd<double> vc = va * vb;
        vc.copy_to(&c[i], stdx::element_aligned);
        modify(a);
        modify(b);
      }
      for (; i < a.size(); ++i) { c[i] = a[i] * b[i]; }
      benchmark::DoNotOptimize(c);
      // Perform vector multiplication here
    }
    state.counters["FLOP"] = {static_cast<double>(2),
                              benchmark::Counter::kIsIterationInvariantRate};
  }

  void bm_vector_multiply_float(benchmark::State & state) {
    constexpr int num_elements = 1000000;
    std::mt19937_64 rng(42);// NOLINT
    std::uniform_real_distribution<float> dist(0.0, 100.0);
    std::vector<float> a = std::views::iota(0, num_elements) |
                           std::views::transform([&](int) { return dist(rng); }) |
                           std::ranges::to<std::vector>();

    std::vector<float> b = std::views::iota(0, num_elements) |
                           std::views::transform([&](int) { return dist(rng); }) |
                           std::ranges::to<std::vector>();
    for (auto _ : state) {
      std::vector<float> c(num_elements);
      for (size_t i = 0; i < a.size(); ++i) {
        c[i] = a[i] * b[i];
        modify(a);
        modify(b);
      }
      benchmark::DoNotOptimize(c);
      // Perform vector multiplication here
    }
    state.counters["FLOP"] = {static_cast<double>(2),
                              benchmark::Counter::kIsIterationInvariantRate};
  }

  void bm_vector_multiply_float_unseq(benchmark::State & state) {
    constexpr int num_elements = 1000000;
    std::mt19937_64 rng(42);// NOLINT
    std::uniform_real_distribution<float> dist(0.0, 100.0);
    std::vector<float> a = std::views::iota(0, num_elements) |
                           std::views::transform([&](int) { return dist(rng); }) |
                           std::ranges::to<std::vector>();

    std::vector<float> b = std::views::iota(0, num_elements) |
                           std::views::transform([&](int) { return dist(rng); }) |
                           std::ranges::to<std::vector>();
    for (auto _ : state) {
      std::vector<float> c(num_elements);
      std::transform(std::execution::unseq, a.begin(), a.end(), b.begin(), c.begin(),
                     [&](float x, float y) {
                       modify(a);
                       modify(b);
                       return x * y;
                     });
      // Perform vector multiplication here in parallel
      benchmark::DoNotOptimize(c);
    }
    state.counters["FLOP"] = {static_cast<double>(2),
                              benchmark::Counter::kIsIterationInvariantRate};
  }

  void bm_vector_multiply_float_simd(benchmark::State & state) {
    namespace stdx = std::experimental;
    constexpr int num_elements = 1000000;
    std::mt19937_64 rng(42);// NOLINT
    std::uniform_real_distribution<float> dist(0.0, 100.0);
    std::vector<float> a = std::views::iota(0, num_elements) |
                           std::views::transform([&](int) { return dist(rng); }) |
                           std::ranges::to<std::vector>();

    std::vector<float> b = std::views::iota(0, num_elements) |
                           std::views::transform([&](int) { return dist(rng); }) |
                           std::ranges::to<std::vector>();
    for (auto _ : state) {
      std::vector<float> c(num_elements);
      std::size_t i = 0;
      for (; i < a.size(); i += stdx::simd<float>::size()) {
        stdx::simd<float> va(&a[i], stdx::element_aligned);
        stdx::simd<float> vb(&b[i], stdx::element_aligned);
        stdx::simd<float> vc = va * vb;
        vc.copy_to(&c[i], stdx::element_aligned);
        modify(a);
        modify(b);
      }
      for (; i < a.size(); ++i) { c[i] = a[i] * b[i]; }
      benchmark::DoNotOptimize(c);
      // Perform vector multiplication here
    }
    state.counters["FLOP"] = {static_cast<double>(2),
                              benchmark::Counter::kIsIterationInvariantRate};
  }

  void bm_vector_multiply_int(benchmark::State & state) {
    constexpr int num_elements = 1000000;
    std::mt19937_64 rng(42);// NOLINT
    std::uniform_int_distribution<int> dist(0, 100);
    std::vector<int> a = std::views::iota(0, num_elements) |
                         std::views::transform([&](int) { return dist(rng); }) |
                         std::ranges::to<std::vector>();

    std::vector<int> b = std::views::iota(0, num_elements) |
                           std::views::transform([&](int) { return dist(rng); }) |
                           std::ranges::to<std::vector>();
    for (auto _ : state) {
      std::vector<int> c(num_elements);
      for (size_t i = 0; i < a.size(); ++i) {
        c[i] = a[i] * b[i];
        modify(a);
        modify(b);
      }
      benchmark::DoNotOptimize(c);
      // Perform vector multiplication here
    }
    state.counters["FLOP"] = {static_cast<double>(2),
                              benchmark::Counter::kIsIterationInvariantRate};
  }

  void bm_vector_multiply_int_unseq(benchmark::State & state) {
    constexpr int num_elements = 1000000;
    std::mt19937_64 rng(42);// NOLINT
    std::uniform_int_distribution<int> dist(0, 100);
    std::vector<int> a = std::views::iota(0, num_elements) |
                         std::views::transform([&](int) { return dist(rng); }) |
                           std::ranges::to<std::vector>();

    std::vector<int> b = std::views::iota(0, num_elements) |
                           std::views::transform([&](int) { return dist(rng); }) |
                           std::ranges::to<std::vector>();
    for (auto _ : state) {
      std::vector<int> c(num_elements);
      std::transform(std::execution::unseq, a.begin(), a.end(), b.begin(), c.begin(),
                     [&](int x, int y) {
                       modify(a);
                       modify(b);
                       return x * y;
                     });
      // Perform vector multiplication here in parallel
      benchmark::DoNotOptimize(c);
    }
    state.counters["FLOP"] = {static_cast<double>(2),
                              benchmark::Counter::kIsIterationInvariantRate};
  }

  void bm_vector_multiply_int_simd(benchmark::State & state) {
    namespace stdx = std::experimental;
    constexpr int num_elements = 1000000;
    std::mt19937_64 rng(42);// NOLINT
    std::uniform_int_distribution<int> dist(0, 100);
    std::vector<int> a = std::views::iota(0, num_elements) |
                           std::views::transform([&](int) { return dist(rng); }) |
                           std::ranges::to<std::vector>();

    std::vector<int> b = std::views::iota(0, num_elements) |
                           std::views::transform([&](int) { return dist(rng); }) |
                           std::ranges::to<std::vector>();
    for (auto _ : state) {
      std::vector<int> c(num_elements);
      std::size_t i = 0;
      for (; i < a.size(); i += stdx::simd<int>::size()) {
        stdx::simd<int> va(&a[i], stdx::element_aligned);
        stdx::simd<int> vb(&b[i], stdx::element_aligned);
        stdx::simd<int> vc = va * vb;
        vc.copy_to(&c[i], stdx::element_aligned);
        modify(a);
        modify(b);
      }
      for (; i < a.size(); ++i) { c[i] = a[i] * b[i]; }
      benchmark::DoNotOptimize(c);
      // Perform vector multiplication here
    }
    state.counters["FLOP"] = {static_cast<double>(2),
                              benchmark::Counter::kIsIterationInvariantRate};
  }

}// namespace
BENCHMARK(bm_vector_multiply_double);
BENCHMARK(bm_vector_multiply_double_unseq);
BENCHMARK(bm_vector_multiply_double_simd);
BENCHMARK(bm_vector_multiply_float);
BENCHMARK(bm_vector_multiply_float_unseq);
BENCHMARK(bm_vector_multiply_float_simd);
BENCHMARK(bm_vector_multiply_int);
BENCHMARK(bm_vector_multiply_int_unseq);
BENCHMARK(bm_vector_multiply_int_simd);

BENCHMARK_MAIN();