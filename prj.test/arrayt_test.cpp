#include <cassert>
#include <iostream>
#include <sstream>
#include <utility>

#include "arrayt/arrayt.hpp"

int main() {
  try {
    ArrayT a;
    assert(a.size() == 0);

    ArrayT b(4);
    assert(b.size() == 4);
    b[0] = 1.0f;
    b[1] = 2.0f;
    assert(b[0] == 1.0f);

    ArrayT c(b);
    assert(c.size() == 4 && c[1] == 2.0f);

    ArrayT d(std::move(c));
    assert(d.size() == 4);
    assert(c.size() == 0);

    ArrayT e;
    e = std::move(d);
    assert(e.size() == 4 && e[1] == 2.0f);
    assert(d.size() == 0);

    e.resize(6);
    assert(e.size() == 6);
    e.insert(2, 9.0f);
    assert(e.size() == 7 && e[2] == 9.0f);
    e.remove(2);
    assert(e.size() == 6);

    e[0] = 3.0f;
    e[1] = 3.0f;
    assert(e.find(3.0f) == 0);
    e.sort();
    assert(e[0] <= e[1]);

    assert(e.find(999.0f) == ArrayT::npos);

    std::ostringstream out;
    e.save_binary(out);
    ArrayT round;
    std::istringstream in(out.str());
    round.load_binary(in);
    assert(round == e);

    ArrayT zero;
    std::ostringstream zout;
    zero.save_binary(zout);
    ArrayT zin;
    std::istringstream zin_s(zout.str());
    zin.load_binary(zin_s);
    assert(zin.empty() && zin == zero);

    std::cout << "OK" << std::endl;
  } catch (...) {
    std::cout << "FAIL" << std::endl;
    return 1;
  }
  return 0;
}
