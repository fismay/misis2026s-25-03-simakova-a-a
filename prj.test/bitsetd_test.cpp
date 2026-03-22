#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "bitsetd/bitsetd.hpp"

int main() {
  try {
    BitsetD empty;
    assert(empty.size() == 0);
    assert(empty.to_string() == "empty");

    BitsetD a(8, false);
    a.set(0, true);
    a.set(4, true);
    assert(static_cast<std::uint32_t>(a) == 0x11u);

    BitsetD b(0x11ull, 8);
    assert(b == a);

    BitsetD c(16, true);
    c.resize(8);
    assert(c.size() == 8);
    assert(static_cast<std::uint32_t>(c) == 0xFFu);

    BitsetD x(4, false);
    x.set(0, true);
    x.set(1, true);
    assert(x.to_string(BitsetD::StrFormat::Bin) == "b0011");
    assert(x.to_string(BitsetD::StrFormat::BinNoPreSep) == "0011");
    assert(x.to_string(BitsetD::StrFormat::Hex).find("0x3") == 0);

    BitsetD p(8, false);
    p.set(1, true);
    p.set(5, true);
    BitsetD q(8, false);
    q.set(1, true);
    q.set(5, true);
    p &= q;
    assert(static_cast<std::uint32_t>(p) == (static_cast<std::uint32_t>(q)));

    BitsetD u(8, false);
    u.set(0, true);
    BitsetD v(8, false);
    v.set(7, true);
    assert(static_cast<std::uint32_t>(u | v) == 0x81u);

    BitsetD xor_a(4, false);
    xor_a.set(0, true);
    xor_a.set(2, true);
    BitsetD xor_b(4, false);
    xor_b.set(1, true);
    xor_b.set(2, true);
    xor_a ^= xor_b;
    assert(static_cast<std::uint32_t>(xor_a) == 0x3u);

    BitsetD n8(8, false);
    n8.set(0, true);
    n8.set(2, true);
    BitsetD inv = ~n8;
    assert(static_cast<std::uint32_t>(inv) == (0xFFu ^ static_cast<std::uint32_t>(n8)));

    BitsetD s(8, true);
    s <<= 2;
    assert(static_cast<std::uint32_t>(s) == 0xFCu);
    s >>= 1;
    assert(static_cast<std::uint32_t>(s) == 0x7Eu);

    BitsetD r(5, false);
    r.set(1, true);
    r.shift(1);
    assert(r.get(0));

    bool threw = false;
    try {
      BitsetD bad(0, true);
    } catch (const std::invalid_argument&) {
      threw = true;
    }
    assert(threw);

    // Копирование
    BitsetD src(12, true);
    src.resize(10);
    BitsetD copy = src;
    assert(copy == src);
    copy.set(0, false);
    assert(copy != src);

    // Перемещение: источник пустой, приёмник сохраняет значение
    BitsetD donor(16, false);
    donor.set(3, true);
    donor.set(9, true);
    const std::uint32_t donor_val = static_cast<std::uint32_t>(donor);
    BitsetD moved_to(std::move(donor));
    assert(moved_to.size() == 16);
    assert(static_cast<std::uint32_t>(moved_to) == donor_val);
    assert(donor.size() == 0);
    assert(donor.to_string() == "empty");

    BitsetD sink;
    BitsetD another(8, true);
    another <<= 4;
    const std::uint32_t another_val = static_cast<std::uint32_t>(another);
    sink = std::move(another);
    assert(sink.size() == 8);
    assert(static_cast<std::uint32_t>(sink) == another_val);
    assert(another.size() == 0);

    BitsetD bin_orig(37, false);
    bin_orig.set(3, true);
    bin_orig.set(36, true);
    std::ostringstream bos;
    bin_orig.write_binary(bos);
    BitsetD bin_read;
    std::istringstream bis(bos.str());
    bin_read.load_binary(bis);
    assert(bin_read == bin_orig);

    BitsetD empty_bs;
    std::ostringstream eos;
    empty_bs.write_binary(eos);
    BitsetD empty_in;
    std::istringstream eis(eos.str());
    empty_in.load_binary(eis);
    assert(empty_in.size() == 0 && empty_in == empty_bs);

    std::cout << "OK" << std::endl;
  } catch (...) {
    std::cout << "FAIL" << std::endl;
    return 1;
  }
  return 0;
}
