// Doctest для ArrayT: пока класс не шаблонный, проверки завязаны на float.
// Если сделаем ArrayT<T>, те же кейсы можно поднять в TEST_CASE_TEMPLATE и
// для float использовать doctest::Approx, для целых — обычное ==.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <utility>

#include "arrayt/arrayt.hpp"

TEST_CASE("arrayt: создание и индексация") {
  ArrayT a(3);
  CHECK(a.size() == 3);
  a[0] = 1.0f;
  a[1] = 2.0f;
  CHECK(a[0] == doctest::Approx(1.0f));
  SUBCASE("после присваивания размер не меняется") { CHECK(a.size() == 3); }
}

TEST_CASE("arrayt: перемещение") {
  ArrayT x(2);
  x[0] = 7.0f;
  ArrayT y(std::move(x));
  CHECK(y.size() == 2);
  CHECK(x.size() == 0);
  CHECK(y[0] == doctest::Approx(7.0f));
}

// Мини-пример: один тест на несколько типов (пригодится после шаблонизации).
TEST_CASE_TEMPLATE("демо: арифметика для нескольких типов (не ArrayT)", T, int, float) {
  T a = T(2);
  T b = T(3);
  CHECK(a + b == T(5));
}
