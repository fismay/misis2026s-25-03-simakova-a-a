// Динамический массив float: свой буфер, resize/insert/remove — как просили в лабе.
#pragma once
#ifndef ARRAYD_ARRAYD_HPP_67676767
#define ARRAYD_ARRAYD_HPP_67676767

#include <cstddef>

class ArrayD {
public:
    ArrayD() = default;
    ArrayD(const ArrayD& other);
    /// Move забирает указатель, в старом объекте остаётся пусто — так проще, чем копировать.
    ArrayD(ArrayD&& other) noexcept;
    ArrayD(const std::ptrdiff_t size);
    ~ArrayD();

    /// Присваивание через копию в аргументе и swap — классика, исключения проще ловить.
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
