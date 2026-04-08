#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "dio/dio.hpp"

namespace {

void roundtrip_text(const std::string& original) {
  std::ostringstream os;
  DioStrT::write(os, original);
  std::istringstream is(os.str());
  std::string got;
  DioStrT::read(is, got);
  assert(got == original);
}

void roundtrip_bin(const std::string& original) {
  std::ostringstream os;
  DioStrB::write(os, original);
  std::istringstream is(os.str());
  std::string got;
  DioStrB::read(is, got);
  assert(got == original);
}

}  // namespace

int main() {
  try {
    roundtrip_text("");
    roundtrip_text("abc");
    roundtrip_text("строка UTF-8");
    roundtrip_text("a\nb\nc");
    roundtrip_text(std::string("x\0y", 3));

    roundtrip_bin("");
    roundtrip_bin("abc");
    roundtrip_bin("строка UTF-8");
    roundtrip_bin("a\nb\nc");
    roundtrip_bin(std::string("x\0y", 3));

    bool threw = false;
    try {
      std::istringstream bad("999999999999999999999\n");
      std::string s;
      DioStrT::read(bad, s);
    } catch (const std::runtime_error&) {
      threw = true;
    }
    assert(threw);

    threw = false;
    try {
      std::istringstream bad2;
      std::string s2;
      DioStrB::read(bad2, s2);
    } catch (const std::runtime_error&) {
      threw = true;
    }
    assert(threw);

    threw = false;
    try {
      std::istringstream bad3("3\nab");
      std::string s3;
      DioStrT::read(bad3, s3);
    } catch (const std::runtime_error&) {
      threw = true;
    }
    assert(threw);

    std::cout << "OK" << std::endl;
  } catch (...) {
    std::cout << "FAIL" << std::endl;
    return 1;
  }
  return 0;
}
