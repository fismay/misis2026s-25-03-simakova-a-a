// Замер времени: сравнение циклического shift() с наивной версией через get/set.

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>

#include "bitsetd/bitsetd.hpp"

namespace {

using Clock = std::chrono::high_resolution_clock;

/// Та же семантика, что у BitsetD::shift, но только через открытый get/set (много вызовов).
void shift_naive_get_set(BitsetD& b, const std::int32_t s) {
  const std::int32_t n = b.size();
  if (n <= 0) return;
  std::int32_t k = s % n;
  if (k < 0) k += n;
  if (k == 0) return;
  const BitsetD tmp(b);
  for (std::int32_t j = 0; j < n; ++j) {
    const std::int32_t from = (j + k) % n;
    b.set(j, tmp.get(from));
  }
}

double ms(const Clock::duration d) {
  return std::chrono::duration<double, std::milli>(d).count();
}

void bench_line(const char* title, const std::int32_t bits, const std::int32_t iterations) {
  BitsetD a(bits, false);
  for (std::int32_t i = 0; i < bits; i += 7) {
    a.set(i % bits, true);
  }

  BitsetD b = a;

  auto t0 = Clock::now();
  for (int i = 0; i < iterations; ++i) {
    a.shift(3);
  }
  auto t1 = Clock::now();

  auto t2 = Clock::now();
  for (int i = 0; i < iterations; ++i) {
    shift_naive_get_set(b, 3);
  }
  auto t3 = Clock::now();

  std::cout << title << " bits=" << bits << " iters=" << iterations << '\n';
  std::cout << "  shift() [chunks]:     " << ms(t1 - t0) << " ms\n";
  std::cout << "  shift_naive get/set:  " << ms(t3 - t2) << " ms\n";
}

}  // namespace

int main() {
  try {
    bench_line("A", 4096, 2000);
    bench_line("B", 16384, 500);

    BitsetD x(64, true);
    auto t0 = Clock::now();
    for (int i = 0; i < 100000; ++i) {
      x.invert();
    }
    auto t1 = Clock::now();
    std::cout << "invert 64 bits x100000: " << ms(t1 - t0) << " ms\n";

    std::cout << "OK" << std::endl;
  } catch (const std::exception& e) {
    std::cout << "FAIL: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
