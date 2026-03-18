// Реализация BitsetD: хранение в uint32_t chunks, младший бит — индекс 0.

#include "bitsetd.hpp"

#include <algorithm>
#include <iomanip>
#include <istream>
#include <ostream>
#include <sstream>
#include <stdexcept>

namespace {

constexpr std::int32_t kChunkBits = 32;

bool raw_get(const std::vector<std::uint32_t>& c, const std::int32_t idx) {
  return (c[static_cast<std::size_t>(idx / kChunkBits)] &
          (UINT32_C(1) << (idx % kChunkBits))) != 0;
}

void raw_set(std::vector<std::uint32_t>& c, const std::int32_t idx, const bool val) {
  std::uint32_t& cell = c[static_cast<std::size_t>(idx / kChunkBits)];
  const std::uint32_t bit = UINT32_C(1) << (idx % kChunkBits);
  if (val) {
    cell |= bit;
  } else {
    cell &= ~bit;
  }
}

}  // namespace

// --- маска «хвоста» последнего chunk ---

std::uint32_t BitsetD::tail_bits_mask(const std::int32_t size) noexcept {
  if (size <= 0) return 0;
  const int32_t rem = size % chunk_bi_s;
  if (rem == 0) return 0xFFFFFFFFu;
  return (UINT32_C(1) << rem) - 1u;
}

void BitsetD::apply_tail_mask() noexcept {
  if (chunks_.empty() || size_ <= 0) return;
  const int32_t last = static_cast<int32_t>(chunks_.size()) - 1;
  chunks_[static_cast<std::size_t>(last)] &= tail_bits_mask(size_);
}

// --- конструкторы ---

BitsetD::BitsetD(const std::int32_t size, const bool val) : size_(size) {
  if (size <= 0) {
    throw std::invalid_argument("BitsetD: size must be positive");
  }
  chunks_.assign(static_cast<std::size_t>(chunks_count()), val ? 0xFFFFFFFFu : 0u);
  apply_tail_mask();
}

BitsetD::BitsetD(const std::uint64_t mask, const std::int32_t size) : size_(size) {
  if (size <= 0) {
    throw std::invalid_argument("BitsetD: size must be positive");
  }
  std::uint64_t m = mask;
  if (size < 64) {
    m &= (UINT64_C(1) << size) - UINT64_C(1);
  }
  chunks_.assign(static_cast<std::size_t>(chunks_count()), 0u);
  if (!chunks_.empty()) {
    chunks_[0] = static_cast<std::uint32_t>(m);
    if (chunks_.size() > 1) {
      chunks_[1] = static_cast<std::uint32_t>(m >> 32);
    }
  }
  apply_tail_mask();
}

BitsetD::BitsetD(BitsetD&& src) noexcept : size_(src.size_), chunks_(std::move(src.chunks_)) { src.size_ = 0; }

BitsetD& BitsetD::operator=(BitsetD&& rhs) noexcept {
  if (this != &rhs) {
    size_ = rhs.size_;
    chunks_ = std::move(rhs.chunks_);
    rhs.size_ = 0;
  }
  return *this;
}

// --- приведение к целым ---

BitsetD::operator std::uint64_t() const {
  if (size_ <= 0 || chunks_.empty()) return 0;
  std::uint64_t low = chunks_[0];
  std::uint64_t high = chunks_.size() > 1 ? chunks_[1] : 0;
  std::uint64_t val = (high << 32) | low;
  if (size_ < 64) {
    val &= (UINT64_C(1) << size_) - UINT64_C(1);
  }
  return val;
}

BitsetD::operator std::uint32_t() const {
  if (size_ <= 0 || chunks_.empty()) return 0;
  std::uint32_t v = chunks_[0];
  if (size_ < chunk_bi_s) {
    v &= tail_bits_mask(size_);
  }
  return v;
}

// --- размер и доступ по индексу ---

void BitsetD::resize(const std::int32_t new_size, const bool val) {
  if (new_size <= 0) {
    throw std::invalid_argument("BitsetD::resize non positive new size");
  }
  const std::int32_t old_size = size_;
  size_ = new_size;
  chunks_.resize(static_cast<std::size_t>(chunks_count()));
  if (old_size < new_size && val) {
    for (std::int32_t i = old_size; i < new_size; ++i) {
      set(i, true);
    }
  } else {
    apply_tail_mask();
  }
}

bool BitsetD::get(const std::int32_t idx) const {
  if (idx < 0 || size_ <= idx) {
    throw std::out_of_range("BitsetD::get invalid index");
  }
  const std::uint32_t w = chunks_[static_cast<std::size_t>(idx / chunk_bi_s)];
  return (w & (UINT32_C(1) << (idx % chunk_bi_s))) != 0;
}

void BitsetD::set(const std::int32_t idx, const bool val) {
  if (idx < 0 || size_ <= idx) {
    throw std::out_of_range("BitsetD::set invalid index");
  }
  std::uint32_t& cell = chunks_[static_cast<std::size_t>(idx / chunk_bi_s)];
  const std::uint32_t bit = UINT32_C(1) << (idx % chunk_bi_s);
  if (val) {
    cell |= bit;
  } else {
    cell &= ~bit;
  }
}

// --- сравнение ---

bool BitsetD::operator==(const BitsetD& rhs) const noexcept {
  if (size_ != rhs.size_) return false;
  const std::int32_t cc = chunks_count();
  if (cc == 0) return true;
  for (std::int32_t i = 0; i < cc - 1; ++i) {
    if (chunks_[static_cast<std::size_t>(i)] != rhs.chunks_[static_cast<std::size_t>(i)]) return false;
  }
  const std::uint32_t m = tail_bits_mask(size_);
  return (chunks_[static_cast<std::size_t>(cc - 1)] & m) ==
         (rhs.chunks_[static_cast<std::size_t>(cc - 1)] & m);
}

// --- поразрядные операции на всём массиве ---

BitsetD& BitsetD::invert() noexcept {
  for (auto& c : chunks_) {
    c = ~c;
  }
  apply_tail_mask();
  return *this;
}

void BitsetD::fill(const bool val) noexcept {
  if (size_ <= 0 || chunks_.empty()) return;
  const auto fillv = val ? 0xFFFFFFFFu : 0u;
  std::fill(chunks_.begin(), chunks_.end(), fillv);
  apply_tail_mask();
}

// Циклический поворот: new[j] = old[(j+k) mod n], k — нормализованный сдвиг (вправо по индексам).
// Прямая работа с chunks_ без виртуальных вызовов get/set на каждый бит.
BitsetD& BitsetD::shift(const std::int32_t s) noexcept {
  const std::int32_t n = size_;
  if (n <= 0) return *this;
  std::int32_t k = s % n;
  if (k < 0) k += n;
  if (k == 0) return *this;
  const BitsetD tmp(*this);
  for (std::int32_t j = 0; j < n; ++j) {
    const std::int32_t from = (j + k) % n;
    raw_set(chunks_, j, raw_get(tmp.chunks_, from));
  }
  apply_tail_mask();
  return *this;
}

// Логические сдвиги: << к большим индексам, >> к меньшим.

BitsetD& BitsetD::operator<<=(const std::int32_t shift) {
  if (shift < 0) return operator>>=(-shift);
  if (size_ <= 0) return *this;
  if (shift == 0) return *this;
  if (shift >= size_) {
    fill(false);
    return *this;
  }
  for (std::int32_t i = size_ - 1; i >= shift; --i) {
    set(i, get(i - shift));
  }
  for (std::int32_t i = 0; i < shift; ++i) {
    set(i, false);
  }
  return *this;
}

BitsetD& BitsetD::operator>>=(const std::int32_t shift) {
  if (shift < 0) return operator<<=(-shift);
  if (size_ <= 0) return *this;
  if (shift == 0) return *this;
  if (shift >= size_) {
    fill(false);
    return *this;
  }
  for (std::int32_t i = 0; i < size_ - shift; ++i) {
    set(i, get(i + shift));
  }
  for (std::int32_t i = size_ - shift; i < size_; ++i) {
    set(i, false);
  }
  return *this;
}

BitsetD& BitsetD::operator&=(const BitsetD& rhs) {
  if (size_ != rhs.size_) {
    throw std::invalid_argument("BitsetD::operator&= different size");
  }
  const std::int32_t cc = chunks_count();
  for (std::int32_t i = 0; i < cc; ++i) {
    chunks_[static_cast<std::size_t>(i)] &= rhs.chunks_[static_cast<std::size_t>(i)];
  }
  apply_tail_mask();
  return *this;
}

BitsetD& BitsetD::operator|=(const BitsetD& rhs) {
  if (size_ != rhs.size_) {
    throw std::invalid_argument("BitsetD::operator|= different size");
  }
  const std::int32_t cc = chunks_count();
  for (std::int32_t i = 0; i < cc; ++i) {
    chunks_[static_cast<std::size_t>(i)] |= rhs.chunks_[static_cast<std::size_t>(i)];
  }
  apply_tail_mask();
  return *this;
}

BitsetD& BitsetD::operator^=(const BitsetD& rhs) {
  if (size_ != rhs.size_) {
    throw std::invalid_argument("BitsetD::operator^= different size");
  }
  const std::int32_t cc = chunks_count();
  for (std::int32_t i = 0; i < cc; ++i) {
    chunks_[static_cast<std::size_t>(i)] ^= rhs.chunks_[static_cast<std::size_t>(i)];
  }
  apply_tail_mask();
  return *this;
}

// --- to_string: двоичный вывод и вспомогательная разбивка по 4 символа (группы от младшего конца строки) ---

// bin_msb_left: символы от старшего бита к младшему; разделители по 4, считая от младшего конца строки
static std::string insert_sep4_from_lsb_side(const std::string& bin_msb_left) {
  const std::size_t L = bin_msb_left.size();
  if (L <= 4) return bin_msb_left;
  std::string out;
  std::size_t i = 0;
  if (L % 4 != 0) {
    out.append(bin_msb_left.substr(0, L % 4));
    i = L % 4;
  } else {
    out.append(bin_msb_left.substr(0, 4));
    i = 4;
  }
  while (i < L) {
    out.push_back('\'');
    out.append(bin_msb_left.substr(i, 4));
    i += 4;
  }
  return out;
}

std::string BitsetD::to_string(const BitsetD::StrFormat fmt, const std::int32_t len) const {
  if (size_ <= 0) return "empty";

  // Двоичный формат Def: префикс "b", при необходимости дополнение нулями слева до len цифр.
  if (fmt == StrFormat::Bin || fmt == StrFormat::Def || fmt == StrFormat::Default) {
    std::string bits;
    bits.reserve(static_cast<std::size_t>(size_));
    for (std::int32_t i = size_ - 1; i >= 0; --i) {
      bits.push_back(get(i) ? '1' : '0');
    }
    std::int32_t min_digits = size_;
    if (len > min_digits) min_digits = len;
    if (static_cast<std::int32_t>(bits.size()) < min_digits) {
      const std::int32_t pad = min_digits - static_cast<std::int32_t>(bits.size());
      bits.insert(0, static_cast<std::size_t>(pad), '0');
    }
    const std::string with_sep = insert_sep4_from_lsb_side(bits);
    return std::string("b") + with_sep;
  }

  if (fmt == StrFormat::BinNoPreSep) {
    std::string bits;
    for (std::int32_t i = size_ - 1; i >= 0; --i) {
      bits.push_back(get(i) ? '1' : '0');
    }
    std::int32_t min_digits = size_;
    if (len > min_digits) min_digits = len;
    if (static_cast<std::int32_t>(bits.size()) < min_digits) {
      const std::int32_t pad = min_digits - static_cast<std::int32_t>(bits.size());
      bits.insert(0, static_cast<std::size_t>(pad), '0');
    }
    return bits;
  }

  // Hex/Oct: при size <= 64 — через числовое значение; иначе — набор цифр от старших битов.
  if (fmt == StrFormat::Hex) {
    static const char* digs = "0123456789ABCDEF";
    std::ostringstream os;
    os << "0x" << std::hex << std::uppercase;
    if (size_ <= 64) {
      os << static_cast<std::uint64_t>(*this);
    } else {
      std::string hex;
      std::int32_t pos = size_ - 1;
      while (pos >= 0) {
        const int take = std::min(4, pos + 1);
        int nibble = 0;
        for (int j = 0; j < take; ++j) {
          nibble = (nibble << 1) | (get(pos - j) ? 1 : 0);
        }
        hex.push_back(digs[nibble]);
        pos -= take;
      }
      std::size_t p = 0;
      while (p + 1 < hex.size() && hex[p] == '0') ++p;
      os << hex.substr(p);
    }
    std::string s = os.str();
    if (len > 0 && static_cast<std::int32_t>(s.size()) < len) {
      s.insert(2, static_cast<std::size_t>(len - static_cast<std::int32_t>(s.size())), '0');
    }
    return s;
  }

  if (fmt == StrFormat::Oct) {
    std::ostringstream os;
    os << '0' << std::oct;
    if (size_ <= 64) {
      os << static_cast<std::uint64_t>(*this);
    } else {
      std::string oct;
      std::int32_t pos = size_ - 1;
      while (pos >= 0) {
        const int take = std::min(3, pos + 1);
        int dig = 0;
        for (int j = 0; j < take; ++j) {
          dig = (dig << 1) | (get(pos - j) ? 1 : 0);
        }
        oct.push_back(static_cast<char>('0' + dig));
        pos -= take;
      }
      std::size_t p = 0;
      while (p + 1 < oct.size() && oct[p] == '0') ++p;
      os << oct.substr(p);
    }
    std::string s = os.str();
    if (len > 0 && static_cast<std::int32_t>(s.size()) < len) {
      s.insert(1, static_cast<std::size_t>(len - static_cast<std::int32_t>(s.size())), '0');
    }
    return s;
  }

  return "empty";
}

void BitsetD::write_binary(std::ostream& os) const {
  static const char kMagic[4] = {'B', 'S', 'D', '1'};
  os.write(kMagic, 4);
  constexpr std::uint32_t kVer = 1;
  os.write(reinterpret_cast<const char*>(&kVer), sizeof(kVer));
  os.write(reinterpret_cast<const char*>(&size_), sizeof(size_));
  const std::uint32_t nc =
      size_ <= 0 ? 0u : static_cast<std::uint32_t>(chunks_count());
  os.write(reinterpret_cast<const char*>(&nc), sizeof(nc));
  if (nc > 0) {
    os.write(reinterpret_cast<const char*>(chunks_.data()),
             static_cast<std::size_t>(nc) * sizeof(std::uint32_t));
  }
}

void BitsetD::load_binary(std::istream& is) {
  auto fail = [this](const char* msg) {
    *this = BitsetD{};
    throw std::runtime_error(msg);
  };

  char mag[4];
  is.read(mag, 4);
  if (!is || mag[0] != 'B' || mag[1] != 'S' || mag[2] != 'D' || mag[3] != '1') {
    fail("BitsetD::load_binary: bad magic");
  }
  std::uint32_t ver = 0;
  is.read(reinterpret_cast<char*>(&ver), sizeof(ver));
  if (!is || ver != 1u) {
    fail("BitsetD::load_binary: unsupported version");
  }
  std::int32_t bits = 0;
  is.read(reinterpret_cast<char*>(&bits), sizeof(bits));
  if (!is || bits < 0) {
    fail("BitsetD::load_binary: invalid bit_count");
  }
  std::uint32_t nc = 0;
  is.read(reinterpret_cast<char*>(&nc), sizeof(nc));
  if (!is) {
    fail("BitsetD::load_binary: truncated header");
  }
  const std::uint32_t expected =
      bits == 0 ? 0u : static_cast<std::uint32_t>((bits + chunk_bi_s - 1) / chunk_bi_s);
  if (nc != expected) {
    fail("BitsetD::load_binary: chunk_count mismatch");
  }

  BitsetD tmp;
  tmp.size_ = bits;
  tmp.chunks_.resize(static_cast<std::size_t>(nc));
  if (nc > 0) {
    is.read(reinterpret_cast<char*>(tmp.chunks_.data()),
            static_cast<std::size_t>(nc) * sizeof(std::uint32_t));
  }
  if (!is) {
    fail("BitsetD::load_binary: truncated payload");
  }
  tmp.apply_tail_mask();
  *this = std::move(tmp);
}
