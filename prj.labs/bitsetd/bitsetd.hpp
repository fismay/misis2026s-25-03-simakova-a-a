// BitsetD — динамический битовый массив. Курс ООП, весна 2026 (МИСИС).
#pragma once
#ifndef BITSETD_BITSETD_HPP_20260214
#define BITSETD_BITSETD_HPP_20260214

#include <cstdint>
#include <iosfwd>
#include <string>
#include <vector>

/// Динамический битовый массив: бит с индексом 0 — младший (как в целочисленном значении).
class BitsetD {
public:
  /// Прокси только для чтения из const BitsetD::operator[].
  /// Хранит снимок бита на момент создания (не отслеживает последующие изменения массива).
  class BitR {
    friend class BitsetD;

  public:
    ~BitR() = default;
    operator bool() const noexcept { return val_; }

  private:
    BitR() = delete;
    BitR(const BitR&) = delete;
    BitR(BitR&&) = delete;
    BitR(const BitsetD& bs, const int32_t idx) : val_(bs.get(idx)) {}
    BitR& operator=(const BitR&) = delete;
    BitR& operator=(BitR&&) = delete;

  private:
    bool val_ = false;
  };

  /// Прокси для записи и чтения из BitsetD::operator[] на неконстантном объекте.
  /// Держит ссылку на массив и индекс; присваивание вызывает set.
  class BitW {
    friend class BitsetD;

  public:
    ~BitW() = default;
    operator bool() const noexcept { return bs_.get(idx_); }
    void operator=(const bool val) noexcept { bs_.set(idx_, val); }
    void operator=(const BitW& rhs) noexcept { operator=(static_cast<bool>(rhs)); }
    void operator=(BitW&& rhs) noexcept { operator=(static_cast<bool>(rhs)); }

  private:
    BitW(BitsetD& bs, const int32_t idx) : bs_(bs), idx_(idx) {}
    BitW() = delete;
    BitW(const BitW&) = delete;
    BitW(BitW&&) = delete;

  private:
    BitsetD& bs_;
    const int32_t idx_ = 0;
  };

public:
  BitsetD() = default;

  BitsetD(const BitsetD& src) = default;

  /// Перемещение забирает буфер; источник приводится к пустому допустимому состоянию (size == 0).
  BitsetD(BitsetD&& src) noexcept;

  /*!
  \param  mask [in] - младшие биты инициализации (до 64 бит)
  \param  size [in] - размер битсета
  \throws std::invalid_argument для неположительного size
  */
  BitsetD(const std::uint64_t mask, const int32_t size);

  /*!
  \param  size [in] - число бит
  \param  val [in] - начальное значение всех бит
  \throws std::invalid_argument для неположительного size
  */
  BitsetD(const int32_t size, const bool val = false);

  ~BitsetD() = default;

  BitsetD& operator=(const BitsetD& rhs) = default;

  BitsetD& operator=(BitsetD&& rhs) noexcept;

  /// Приведение к целому: младшие size_ бит; не более 64/32 значимых бит соответственно.
  explicit operator std::uint64_t() const;

  explicit operator std::uint32_t() const;

  std::int32_t size() const noexcept { return size_; }

  void resize(const std::int32_t new_size, const bool val = false);

  /*!
  Прочитать значение бита по указанному индексу.
  \param  idx[in] - индекс бита
  \return Значение бита
  \throws std::out_of_range при выходе индекса за пределы допустимого
  */
  bool get(const std::int32_t idx) const;

  /*!
  Установить значение бита по указанному индексу.
  \param idx[in] - индекс бита
  \param val[in] - значение бита
  \throws std::out_of_range при выходе индекса за пределы допустимого
  */
  void set(const std::int32_t idx, const bool val);

  /// Индексация: на неконстантном объекте — записываемый прокси BitW, на const — BitR.
  BitW operator[](const std::int32_t idx) & { return BitW(*this, idx); }

  BitR operator[](const std::int32_t idx) const & { return BitR(*this, idx); }

  bool operator==(const BitsetD& rhs) const noexcept;

  BitsetD& invert() noexcept;

  void fill(const bool val) noexcept;

  /// Циклический сдвиг по кольцу [0, size_): положительный s — поворот «вправо» по индексам.
  BitsetD& shift(const std::int32_t s) noexcept;

  /// Логический сдвиг к старшим индексам; освободившиеся младшие биты — 0.
  BitsetD& operator<<=(const std::int32_t shift);

  /// Логический сдвиг к младшим индексам; освободившиеся старшие биты — 0.
  BitsetD& operator>>=(const std::int32_t shift);

  BitsetD& operator&=(const BitsetD& rhs);

  BitsetD& operator|=(const BitsetD& rhs);

  BitsetD& operator^=(const BitsetD& rhs);

  enum class StrFormat : int32_t {
    Bin = 1,      ///< суффикс "b0" и разделители по 4 от младших, пример: b0010'0001
    BinNoPreSep,  ///< без суффикса и разделителей, пример: 0100001
    Oct,
    Hex,
    Def = Bin,
    Default = Def
  };

  /*!
  Форматированное текстовое представление, "empty" - для пустого
  \param fmt [in] - формат представления
  \param len [in] - минимальная длина строки в символах выбранного формата
  */
  std::string to_string(const StrFormat fmt = StrFormat::Def, const int32_t len = 0) const;

  /*!
  Бинарный формат v1 (little-endian, как в памяти на типичных ПК):
  4 байта магии "BSD1", uint32_t version=1, int32_t bit_count,
  uint32_t chunk_count, затем chunk_count значений uint32_t подряд (сырой chunks_).
  */
  void write_binary(std::ostream& os) const;

  /// Заменяет содержимое; при ошибке формата — std::runtime_error, объект можно считать пустым.
  void load_binary(std::istream& is);

private:
  std::int32_t size_ = 0;                     ///< число значимых бит
  std::vector<std::uint32_t> chunks_;       ///< упаковка по 32 бита; бит i в chunk i/32, позиция i%32

  static const int32_t chunk_bi_s = 32;       ///< бит в одном элементе chunks_
  static const int32_t chunk_by_s = 4;        ///< байт в одном элементе chunks_

  int32_t chunks_count() const { return (size_ + chunk_bi_s - 1) / chunk_bi_s; }

  /// Обнулить биты выше size_ в последнем chunk (после инверсий и т.п.).
  void apply_tail_mask() noexcept;
  /// Маска для младших rem бит последнего chunk, rem = size % 32 (или все 32, если rem == 0).
  static std::uint32_t tail_bits_mask(std::int32_t size) noexcept;
};

// Свободные операторы: работают через копию lhs (исходный BitsetD не меняют).

inline BitsetD operator~(const BitsetD& val) noexcept { return BitsetD(val).invert(); }

inline BitsetD operator<<(const BitsetD& lhs, const std::int32_t shift) {
  return BitsetD(lhs).operator<<=(shift);
}

inline BitsetD operator>>(const BitsetD& lhs, const std::int32_t shift) {
  return BitsetD(lhs).operator>>=(shift);
}

inline BitsetD operator&(const BitsetD& lhs, const BitsetD& rhs) {
  return BitsetD(lhs).operator&=(rhs);
}

inline BitsetD operator|(const BitsetD& lhs, const BitsetD& rhs) {
  return BitsetD(lhs).operator|=(rhs);
}

inline BitsetD operator^(const BitsetD& lhs, const BitsetD& rhs) {
  return BitsetD(lhs).operator^=(rhs);
}

#endif
