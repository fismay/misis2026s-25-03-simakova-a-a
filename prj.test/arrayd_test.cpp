#include <cassert>
#include <iostream>
#include <utility>

#include "arrayd/arrayd.hpp"

int main() {
  try {
    ArrayD a;
    assert(a.size() == 0);

    ArrayD b(5);
    assert(b.size() == 5);
    b[0] = 1.0f;
    b[2] = 4.0f;
    assert(b[0] == 1.0f);

    ArrayD c(b);
    assert(c.size() == 5 && c[2] == 4.0f);

    ArrayD d(std::move(c));
    assert(d.size() == 5 && d[2] == 4.0f);
    assert(c.size() == 0);

    ArrayD e;
    e = std::move(d);
    assert(e.size() == 5 && e[0] == 1.0f);
    assert(d.size() == 0);

    e.resize(3);
    assert(e.size() == 3);
    e.resize(8);
    assert(e.size() == 8);
    e.insert(1, 9.0f);
    assert(e.size() == 9 && e[1] == 9.0f);
    e.remove(1);
    assert(e.size() == 8);

    e[0] = 2.0f;
    e[1] = 2.0f;
    assert(e.find(2.0f) == 0);
    e.sort();
    assert(e[0] <= e[1]);

    ArrayD copy_assign;
    copy_assign = e;
    assert(copy_assign.size() == e.size());
    for (std::ptrdiff_t i = 0; i < e.size(); ++i) assert(copy_assign[i] == e[i]);
    copy_assign[0] = -1.0f;
    assert(copy_assign[0] != e[0]);

    std::cout << "OK" << std::endl;
  } catch (...) {
    std::cout << "FAIL" << std::endl;
    return 1;
  }
  return 0;
}
