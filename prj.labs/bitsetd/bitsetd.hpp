// Битовый массив на uint32_t кусках; младший бит — как в числе, не перепутать с «строкой битов».
// Здесь объявления и простые inline, тяжёлое в .cpp.
#pragma once
#ifndef BITSETD_BITSETD_HPP_67676767
#define BITSETD_BITSETD_HPP_67676767

#include <cstdint>
#include <iosfwd>
#include <string>
#include <vector>

/// Динамический битовый массив: бит с индексом 0 — младший (как в целочисленном значении).
class BitsetD {
public:
  /// Из const [] возвращаю такой маленький объект — там уже зафиксировано true/false на момент вызова.
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

    bool val_ = false;
  };

  /// Для неконстантного [] — держу ссылку на массив и индекс, присваивание = set.
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

    BitsetD& bs_;
    const int32_t idx_ = 0;
  };

public:
  BitsetD() = default;

  BitsetD(const BitsetD& src) = default;

  BitsetD(BitsetD&& src) noexcept;

  BitsetD(const std::uint64_t mask, const int32_t size);

  BitsetD(const int32_t size, const bool val = false);

  ~BitsetD() = default;

  BitsetD& operator=(const BitsetD& rhs) = default;

  BitsetD& operator=(BitsetD&& rhs) noexcept;

  explicit operator std::uint64_t() const;

  explicit operator std::uint32_t() const;

  std::int32_t size() const noexcept { return size_; }

  void resize(const std::int32_t new_size, const bool val = false);

  bool get(const std::int32_t idx) const;

  void set(const std::int32_t idx, const bool val);

  BitW operator[](const std::int32_t idx) & { return BitW(*this, idx); }

  BitR operator[](const std::int32_t idx) const & { return BitR(*this, idx); }

  bool operator==(const BitsetD& rhs) const noexcept;

  BitsetD& invert() noexcept;

  void fill(const bool val) noexcept;

  BitsetD& shift(const std::int32_t s) noexcept;

  BitsetD& operator<<=(const std::int32_t shift);

  BitsetD& operator>>=(const std::int32_t shift);

  BitsetD& operator&=(const BitsetD& rhs);

  BitsetD& operator|=(const BitsetD& rhs);

  BitsetD& operator^=(const BitsetD& rhs);

  enum class StrFormat : int32_t {
    Bin = 1,
    BinNoPreSep,
    Oct,
    Hex,
    Def = Bin,
    Default = Def
  };

  std::string to_string(const StrFormat fmt = StrFormat::Def, const int32_t len = 0) const;

  void write_binary(std::ostream& os) const;

  void load_binary(std::istream& is);

private:
  std::int32_t size_ = 0;
  std::vector<std::uint32_t> chunks_;

private:
  static const int32_t chunk_bi_s = 32;
  static const int32_t chunk_by_s = 4;

  int32_t chunks_count() const { return (size_ + chunk_bi_s - 1) / chunk_bi_s; }

  static std::uint32_t tail_bits_mask(std::int32_t size) noexcept {
    if (size <= 0) return 0;
    const int32_t rem = size % chunk_bi_s;
    if (rem == 0) return 0xFFFFFFFFu;
    return (UINT32_C(1) << rem) - 1u;
  }

  void apply_tail_mask() noexcept;
};

inline BitsetD::operator std::uint64_t() const {
  if (size_ <= 0 || chunks_.empty()) return 0;
  std::uint64_t low = chunks_[0];
  std::uint64_t high = chunks_.size() > 1 ? chunks_[1] : 0;
  std::uint64_t val = (high << 32) | low;
  if (size_ < 64) {
    val &= (UINT64_C(1) << size_) - UINT64_C(1);
  }
  return val;
}

inline BitsetD::operator std::uint32_t() const {
  if (size_ <= 0 || chunks_.empty()) return 0;
  std::uint32_t v = chunks_[0];
  if (size_ < BitsetD::chunk_bi_s) {
    v &= BitsetD::tail_bits_mask(size_);
  }
  return v;
}

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
