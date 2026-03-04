#pragma once
#ifndef ARRAYD_ARRAYD_HPP_525252
#define ARRAYD_ARRAYD_HPP_525252

#include <cstddef>

class ArrayD {
public:
    ArrayD() = default;
    ArrayD(const ArrayD& other);
    /// Перемещение: забирает буфер, источник остаётся пустым (nullptr, size 0).
    ArrayD(ArrayD&& other) noexcept;
    ArrayD(const std::ptrdiff_t size);
    ~ArrayD();

    /// Копирование и перемещение: приёмник получает состояние other (через обмен с временной копией).
    ArrayD& operator=(ArrayD other);
    void swap(ArrayD& other) noexcept;

    [[nodiscard]] std::ptrdiff_t size() const noexcept { return size_; }

    void resize(const std::ptrdiff_t new_size);

    [[nodiscard]] float& operator[](const std::ptrdiff_t idx);
    [[nodiscard]] float operator[](const std::ptrdiff_t idx) const;

    void insert(const std::ptrdiff_t idx, const float val);
    void remove(const std::ptrdiff_t idx);

    [[nodiscard]] std::ptrdiff_t find(const float val) const noexcept;
    void sort() noexcept;

private:
    std::ptrdiff_t capacity_ = 0;
    std::ptrdiff_t size_ = 0;
    float* data_ = nullptr;
};

#endif
