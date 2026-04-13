// Строка в поток: сначала сделала текстовый вариант (длина + перевод строки), потом бинарный с uint64.
// Удобно гонять через stringstream в тестах.
#pragma once
#ifndef DIO_DIO_HPP_67676767
#define DIO_DIO_HPP_67676767

#include <cstdint>
#include <istream>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <string>

/// Текстовый вариант: сверху пишу длину числом и перевод строки, ниже — сырой кусок байт (и с \\n внутри можно).
class DioStrT {
public:
  static void write(std::ostream& os, const std::string& s) {
    os << s.size() << '\n';
    if (!s.empty()) {
      os.write(s.data(), static_cast<std::streamsize>(s.size()));
    }
  }

  /// Читаю в переданную строку; если поток оборван или формат кривой — кидаю runtime_error и очищаю out.
  static void read(std::istream& is, std::string& out) {
    std::size_t n = 0;
    if (!(is >> n)) {
      throw std::runtime_error("DioStrT::read: не удалось прочитать длину");
    }
    if (n > static_cast<std::size_t>(std::numeric_limits<std::string::size_type>::max())) {
      throw std::runtime_error("DioStrT::read: слишком большая длина");
    }
    if (is.get() != '\n') {
      throw std::runtime_error("DioStrT::read: ожидался перевод строки после длины");
    }
    out.assign(static_cast<std::string::size_type>(n), '\0');
    if (n > 0) {
      is.read(out.data(), static_cast<std::streamsize>(n));
      if (static_cast<std::size_t>(is.gcount()) != n) {
        out.clear();
        throw std::runtime_error("DioStrT::read: неполная полезная нагрузка");
      }
    }
  }
};

/// Бинарный вариант: 8 байт длины (uint64, как у меня в памяти), потом подряд байты строки.
class DioStrB {
public:
  static void write(std::ostream& os, const std::string& s) {
    const auto n = static_cast<std::uint64_t>(s.size());
    if (static_cast<std::size_t>(n) != s.size()) {
      throw std::runtime_error("DioStrB::write: длина строки не помещается в uint64_t");
    }
    os.write(reinterpret_cast<const char*>(&n), sizeof(n));
    if (!s.empty()) {
      os.write(s.data(), static_cast<std::streamsize>(s.size()));
    }
  }

  static void read(std::istream& is, std::string& out) {
    std::uint64_t n = 0;
    is.read(reinterpret_cast<char*>(&n), sizeof(n));
    if (!is) {
      throw std::runtime_error("DioStrB::read: не удалось прочитать длину");
    }
    if (n > static_cast<std::uint64_t>(std::numeric_limits<std::string::size_type>::max())) {
      throw std::runtime_error("DioStrB::read: слишком большая длина");
    }
    const auto len = static_cast<std::string::size_type>(n);
    out.assign(len, '\0');
    if (len > 0) {
      is.read(out.data(), static_cast<std::streamsize>(len));
      if (static_cast<std::string::size_type>(is.gcount()) != len) {
        out.clear();
        throw std::runtime_error("DioStrB::read: неполная полезная нагрузка");
      }
    }
  }
};

#endif
