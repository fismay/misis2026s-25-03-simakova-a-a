#pragma once
#ifndef DIO_DIO_HPP_2026
#define DIO_DIO_HPP_2026

#include <cstdint>
#include <istream>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <string>

/// Запись и чтение std::string в потоки: текстовый префикс (длина) и бинарный блок длины + байты.

/// Текстовый формат: строка из десятичной записи `size` (без ведущих нулей), символ перевода строки `\n`,
/// затем ровно `size` байт полезной нагрузки (в т.ч. с `\0`, `\n` и произвольной UTF-8).
class DioStrT {
public:
  static void write(std::ostream& os, const std::string& s) {
    os << s.size() << '\n';
    if (!s.empty()) {
      os.write(s.data(), static_cast<std::streamsize>(s.size()));
    }
  }

  /// Подменяет содержимое `out`; при ошибке формата или обрыве потока — `std::runtime_error`.
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

/// Бинарный формат v1: `uint64_t` длины (как в памяти на little-endian машинах), затем `size` байт подряд.
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
