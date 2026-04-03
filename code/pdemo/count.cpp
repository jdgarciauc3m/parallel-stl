#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstddef>
#include <execution>
#include <fstream>
#include <numeric>
#include <print>
#include <ranges>
#include <string>

#include <oneapi/dpl/execution>
#include <oneapi/dpl/ranges>
#include <string_view>

namespace {
  std::string read_file(const std::string & filename) {
    std::ifstream file {filename};
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
  }

  size_t is_letter(char c) {
    return static_cast<std::size_t>(std::isalpha(static_cast<int>(c)) != 0);
  }

  size_t is_new_word(char c1, char c2) {
    return static_cast<std::size_t>((is_letter(c1) == 0) and (is_letter(c2) != 0));
  }

  size_t count_classic(std::string_view text) {
    if (text.empty()) { return 0; }
    size_t result = 0;
    for (size_t i = 1; i < text.size(); ++i) {
      if (is_new_word(text[i - 1], text[i]) != 0) { ++result; }
    }
    return result + is_letter(text[0]);
  }

  size_t count_classic_stl(std::string_view text) {
    if (text.empty()) { return 0; }
    auto result =
        std::transform_reduce(std::begin(text), std::prev(std::end(text)),
                              std::next(std::begin(text)), 0ZU, std::plus {}, is_new_word);
    return result + is_letter(text[0]);
  }

  size_t count_parstl(std::string_view text) {
    if (text.empty()) { return 0; }
    auto result =
        std::transform_reduce(std::execution::par_unseq, std::begin(text), std::prev(std::end(text)),
                              std::next(std::begin(text)), 0ZU, std::plus {}, is_new_word);
    return result + is_letter(text[0]);
  }

  size_t count_ranges(std::string_view text) {
    if (text.empty()) { return 0; }
    auto compute =
        std::ranges::views::zip(text | std::ranges::views::take(std::ranges::size(text) - 1),
                                text | std::ranges::views::drop(1));
    auto result = std::ranges::count_if(compute, [](auto char_pair) {
      auto [c1, c2] = char_pair;
      return is_new_word(c1, c2);
    });
    return result + is_letter(text[0]);
  }

  size_t count_pranges(std::string_view text) {
    namespace stdx = oneapi::dpl;

    if (text.empty()) { return 0; }
    auto compute =
        std::ranges::views::zip(text | std::ranges::views::take(std::ranges::size(text) - 1),
                                text | std::ranges::views::drop(1));
    auto result = stdx::ranges::count_if(stdx::execution::par_unseq, compute, [](auto char_pair) {
      auto [c1, c2] = char_pair;
      return is_new_word(c1, c2);
    });
    return result + static_cast<size_t>(is_letter(text[0]) != 0);
  }

  auto run_test(auto count_fun, std::string_view text) {
    constexpr size_t max_iterations = 30;
    size_t total_time = 0;
    size_t result = 0;
    for (size_t i = 0; i < max_iterations; ++i) {
      auto t1 = std::chrono::high_resolution_clock::now();
      result = count_fun(text);
      auto t2 = std::chrono::high_resolution_clock::now();
      auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
      total_time += time.count();
    }
    return std::tuple {static_cast<double>(total_time) / max_iterations, result};
  }

}// namespace


int main() {
  std::string text = read_file("quijote.txt");
  std::println("Number of characters: {}", text.size());

  auto [time_classic, result_classic] = run_test(count_classic, text);
  std::println("Result: {}, Average time (classic): {}", result_classic, time_classic / 1000000.0);

  auto [time_classicstl, result_classicstl] = run_test(count_classic_stl, text);
  auto [time_parstl, result_parstl] = run_test(count_parstl, text);
  auto [time_ranges, result_ranges] = run_test(count_ranges, text);
  auto [time_pranges, result_pranges] = run_test(count_pranges, text);

  std::println("Result: {}, Average time (classic stl): {}", result_classicstl,
               time_classicstl / 1000000.0);
  std::println("Result: {}, Average time (parstl): {}, Speedup: {}", result_parstl,
               time_parstl / 1000000.0, time_classicstl / time_parstl);
  std::println("Result: {}, Average time (ranges): {}, Speedup: {}", result_ranges,
               time_ranges / 1000000.0, time_classicstl / time_ranges);
  std::println("Result: {}, Average time (pranges): {}, Speedup: {}", result_pranges,
               time_pranges / 1000000.0, time_classicstl / time_pranges);
}