#pragma once
#ifndef ARRAYT_ARRAYT_HPP_2026
#define ARRAYT_ARRAYT_HPP_2026

/// Заготовка под шаблон: тип элемента и тип индекса/размера вынесены в typedef.
/// Дальнейший шаг: `template <typename T, typename S> class ArrayT` с тем же кодом.
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <istream>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <utility>

class ArrayT {
public:
  using value_type = float;
  using size_type = std::ptrdiff_t;

  /// Значение, возвращаемое find, если элемент не найден (как у std::string::npos).
  static constexpr size_type npos = static_cast<size_type>(-1);

  ArrayT() = default;

  ArrayT(const ArrayT& other) : capacity_(other.size_), size_(other.size_) {
    if (capacity_ > 0) {
      data_ = new value_type[static_cast<std::size_t>(capacity_)];
      std::memcpy(data_, other.data_, static_cast<std::size_t>(size_) * sizeof(value_type));
    }
  }

  ArrayT(ArrayT&& other) noexcept
      : capacity_(other.capacity_), size_(other.size_), data_(other.data_) {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
  }

  explicit ArrayT(const size_type size) {
    if (size < 0) throw std::invalid_argument("ArrayT: size cannot be negative");
    if (size > 0) {
      capacity_ = size;
      size_ = size;
      data_ = new value_type[static_cast<std::size_t>(capacity_)]();
    }
  }

  ~ArrayT() { delete[] data_; }

  ArrayT& operator=(ArrayT other) noexcept {
    swap(other);
    return *this;
  }

  void swap(ArrayT& other) noexcept {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
  }

  [[nodiscard]] size_type size() const noexcept { return size_; }

  [[nodiscard]] bool empty() const noexcept { return size_ == 0; }

  [[nodiscard]] bool operator==(const ArrayT& rhs) const noexcept {
    if (size_ != rhs.size_) return false;
    if (size_ == 0) return true;
    return std::memcmp(data_, rhs.data_, static_cast<std::size_t>(size_) * sizeof(value_type)) == 0;
  }

  [[nodiscard]] bool operator!=(const ArrayT& rhs) const noexcept { return !(*this == rhs); }

  void resize(const size_type new_size) {
    if (new_size < 0) throw std::invalid_argument("ArrayT: new size cannot be negative");

    if (new_size > capacity_) {
      size_type new_capacity = std::max(new_size, capacity_ * 2);
      if (capacity_ == 0) new_capacity = std::max(new_size, static_cast<size_type>(2));

      value_type* new_data = new value_type[static_cast<std::size_t>(new_capacity)]();

      if (data_) {
        std::memcpy(new_data, data_, static_cast<std::size_t>(size_) * sizeof(value_type));
        delete[] data_;
      }
      data_ = new_data;
      capacity_ = new_capacity;
    } else if (new_size > size_) {
      std::memset(data_ + size_, 0,
                  static_cast<std::size_t>(new_size - size_) * sizeof(value_type));
    }
    size_ = new_size;
  }

  [[nodiscard]] value_type& operator[](const size_type idx) {
    if (idx < 0 || idx >= size_) throw std::out_of_range("ArrayT: index out of range");
    return data_[idx];
  }

  [[nodiscard]] value_type operator[](const size_type idx) const {
    if (idx < 0 || idx >= size_) throw std::out_of_range("ArrayT: index out of range");
    return data_[idx];
  }

  void insert(const size_type idx, const value_type val) {
    if (idx < 0 || idx > size_) throw std::out_of_range("ArrayT: invalid insert index");

    const size_type old_size = size_;
    resize(size_ + 1);

    if (idx < old_size) {
      std::memmove(data_ + idx + 1, data_ + idx,
                   static_cast<std::size_t>(old_size - idx) * sizeof(value_type));
    }
    data_[idx] = val;
  }

  void remove(const size_type idx) {
    if (idx < 0 || idx >= size_) throw std::out_of_range("ArrayT: invalid remove index");

    if (idx < size_ - 1) {
      std::memmove(data_ + idx, data_ + idx + 1,
                   static_cast<std::size_t>(size_ - idx - 1) * sizeof(value_type));
    }
    size_--;
  }

  [[nodiscard]] size_type find(const value_type val) const noexcept {
    for (size_type i = 0; i < size_; ++i) {
      if (data_[i] == val) return i;
    }
    return npos;
  }

  /*!
  Бинарный формат v1 (LE, как в памяти на типичных ПК):
  магия "ART1", uint32_t version=1, int64_t elem_count, затем подряд sizeof(float)*count байт.
  */
  void save_binary(std::ostream& os) const;

  /// Заменяет содержимое; при ошибке — std::runtime_error, затем пустой массив.
  void load_binary(std::istream& is);

  void sort() noexcept {
    if (size_ > 1) {
      std::sort(data_, data_ + size_);
    }
  }

private:
  size_type capacity_ = 0;
  size_type size_ = 0;
  value_type* data_ = nullptr;
};

inline void ArrayT::save_binary(std::ostream& os) const {
  static const char kMagic[4] = {'A', 'R', 'T', '1'};
  os.write(kMagic, 4);
  constexpr std::uint32_t kVer = 1;
  os.write(reinterpret_cast<const char*>(&kVer), sizeof(kVer));
  const std::int64_t n = static_cast<std::int64_t>(size_);
  os.write(reinterpret_cast<const char*>(&n), sizeof(n));
  if (size_ > 0) {
    os.write(reinterpret_cast<const char*>(data_),
             static_cast<std::size_t>(size_) * sizeof(value_type));
  }
}

inline void ArrayT::load_binary(std::istream& is) {
  auto fail = [](ArrayT& self, const char* msg) {
    self = ArrayT{};
    throw std::runtime_error(msg);
  };

  char mag[4];
  is.read(mag, 4);
  if (!is || mag[0] != 'A' || mag[1] != 'R' || mag[2] != 'T' || mag[3] != '1') {
    fail(*this, "ArrayT::load_binary: bad magic");
  }
  std::uint32_t ver = 0;
  is.read(reinterpret_cast<char*>(&ver), sizeof(ver));
  if (!is || ver != 1u) {
    fail(*this, "ArrayT::load_binary: unsupported version");
  }
  std::int64_t n64 = 0;
  is.read(reinterpret_cast<char*>(&n64), sizeof(n64));
  if (!is || n64 < 0 ||
      n64 > static_cast<std::int64_t>(std::numeric_limits<size_type>::max())) {
    fail(*this, "ArrayT::load_binary: invalid size");
  }
  const size_type n = static_cast<size_type>(n64);
  ArrayT tmp;
  tmp.resize(n);
  if (n > 0) {
    is.read(reinterpret_cast<char*>(tmp.data_), static_cast<std::size_t>(n) * sizeof(value_type));
  }
  if (!is) {
    fail(*this, "ArrayT::load_binary: truncated payload");
  }
  *this = std::move(tmp);
}

#endif
