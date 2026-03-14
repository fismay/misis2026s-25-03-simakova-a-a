// Песочница doctest: сценарии для будущего шаблонного ArrayT<T, S>.
// Сейчас ArrayT — нетипизированный класс с typedef value_type / size_type внутри.
//
// «Завязка» текущих проверок на float:
//   литералы 1.0f, сравнение через == — осмысленны для float.
// Как развязать при шаблонизации:
//   - вынести эталонные значения в переменные типа T или в constexpr-константы;
//   - для сравнения с погрешностью использовать doctest::Approx для float/double
//     и точное == для целых;
//   - TEST_CASE_TEMPLATE / TYPED_TEST позволяют прогнать одни и те же проверки
//     для T = float, int, … если контракт одинаков (find, sort требуют operator< и ==).

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

// Демонстрация: doctest прогоняет один и тот же код для списка типов.
// Для настоящего шаблонного массива сюда переносятся проверки, не зависящие от T.
TEST_CASE_TEMPLATE("демо: арифметика для нескольких типов (не ArrayT)", T, int, float) {
  T a = T(2);
  T b = T(3);
  CHECK(a + b == T(5));
}
