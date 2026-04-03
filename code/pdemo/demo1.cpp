#include <algorithm>
#include <chrono>
#include <execution>
#include <print>
#include <random>
#include <ranges>
#include <tuple>
#include <vector>

#include <tbb/tbb.h>
#include <oneapi/dpl/execution>
#include <oneapi/dpl/ranges>

namespace {

  void warm_up_tbb() {
    tbb::parallel_for(0, tbb::info::default_concurrency(), [=](int) {
      tbb::tick_count t0 = tbb::tick_count::now();
      while ((tbb::tick_count::now() - t0).seconds() < 0.01) {}
    });
  }

  std::vector<double> generate_values(size_t n) {
    std::mt19937_64 rng {std::random_device {}()};
    std::uniform_real_distribution<double> dist(0.0, 1000.0);
    std::vector<double> values(n);
    // std::ranges::generate(values, [&]() { return dist(rng); });
    values = std::views::iota(0U, n) | std::views::transform([&](size_t) { return dist(rng); }) |
             std::ranges::to<std::vector>();
    values.insert(values.begin(), 10'000.0); // Ensure at least one value has a square greater than 500000.0
    return values;
  }

  auto run_test() {
    auto v = generate_values(10'000'000);
    auto compute = v | std::views::transform([](double x) { return std::tuple {x, x * x}; }) |
                   std::views::reverse;
    auto t1 = std::chrono::high_resolution_clock::now();
    auto result = std::ranges::find_if(compute, [](auto x) { return std::get<1>(x) > 100'000'000.0-1; });
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
    std::println("Time taken to find the first value with square greater than 100'000'000.0-1: {}",
                 duration);
    if (result != compute.end()) {
      std::println("First value with square greater than 100'000'000.0-1: {}", std::get<0>(*result));
    } else {
      std::println("No value greater than 100'000'000.0-1 found.");
    }
    return duration.count();
  }

  auto run_test2() {
    auto v = generate_values(10'000'000);
    auto compute = v | std::views::transform([](double x) { return std::tuple {x, x * x}; }) |
                   std::views::reverse;
    auto t1 = std::chrono::high_resolution_clock::now();
    auto result = std::find_if(std::execution::par, compute.begin(), compute.end(),
                               [](auto x) { return std::get<1>(x) > 100'000'000.0 - 1; });
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
    std::println("Time taken to find the first value with square greater than 100'000'000.0-1: {}",
                 duration);
    if (result != compute.end()) {
      std::println("First value with square greater than 100'000'000.0-1: {}", std::get<0>(*result));
    } else {
      std::println("No value greater than 100'000'000.0-1 found.");
    }
    return duration.count();
  }

  auto run_test3() {
    namespace stdx = oneapi::dpl;
    auto v = generate_values(10'000'000);
    auto compute = v | std::views::transform([](double x) { return std::tuple {x, x * x}; }) |
                   std::views::reverse;
    auto t1 = std::chrono::high_resolution_clock::now();
    auto result = stdx::ranges::find_if(stdx::execution::par, compute,
                               [](auto x) { return std::get<1>(x) > 100'000'000.0 - 1; });
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
    std::println("Time taken to find the first value with square greater than 100'000'000.0-1: {}",
                 duration);
    if (result != compute.end()) {
      std::println("First value  with square greater than 100'000'000.0-1: {}", std::get<0>(*result));
    } else {
      std::println("No value greater than 100'000'000.0-1 found.");
    }
    return duration.count();
  }

}// namespace

int main() {
  warm_up_tbb();
  auto base = run_test();
  auto tpstl = run_test2();
  auto tdpl = run_test3();
  std::println("PSTL speedup: {}x", static_cast<double>(base) / static_cast<double>(tpstl));
  std::println("DPL speedup: {}x", static_cast<double>(base) / static_cast<double>(tdpl));
}