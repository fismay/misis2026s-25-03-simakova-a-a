#include "arrayd.hpp"
#include <algorithm>
#include <stdexcept>
#include <cstring>

ArrayD::ArrayD(const ArrayD& other)
    : capacity_(other.size_), size_(other.size_) {
    if (capacity_ > 0) {
        data_ = new float[capacity_];
        std::memcpy(data_, other.data_, size_ * sizeof(float));
    }
}

ArrayD::ArrayD(ArrayD&& other) noexcept
    : capacity_(other.capacity_), size_(other.size_), data_(other.data_) {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}

ArrayD::ArrayD(const std::ptrdiff_t size) {
    if (size < 0) throw std::invalid_argument("ArrayD: size cannot be negative");
    if (size > 0) {
        capacity_ = size;
        size_ = size;
        data_ = new float[capacity_]();
    }
}

ArrayD::~ArrayD() {
    delete[] data_;
}

void ArrayD::swap(ArrayD& other) noexcept {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}

ArrayD& ArrayD::operator=(ArrayD other) {
    swap(other);
    return *this;
}

void ArrayD::resize(const std::ptrdiff_t new_size) {
    if (new_size < 0) throw std::invalid_argument("ArrayD: new size cannot be negative");

    if (new_size > capacity_) {
        // Fix: корректная обработка нулевой емкости
        std::ptrdiff_t new_capacity = std::max(new_size, capacity_ * 2);
        if (capacity_ == 0) new_capacity = std::max(new_size, static_cast<std::ptrdiff_t>(2));
        
        float* new_data = new float[new_capacity]();

        if (data_) {
            std::memcpy(new_data, data_, size_ * sizeof(float));
            delete[] data_;
        }
        data_ = new_data;
        capacity_ = new_capacity;
    }
    else if (new_size > size_) {
        std::memset(data_ + size_, 0, (new_size - size_) * sizeof(float));
    }
    size_ = new_size;
}

float& ArrayD::operator[](const std::ptrdiff_t idx) {
    if (idx < 0 || idx >= size_) throw std::out_of_range("ArrayD: index out of range");
    return data_[idx];
}

float ArrayD::operator[](const std::ptrdiff_t idx) const {
    if (idx < 0 || idx >= size_) throw std::out_of_range("ArrayD: index out of range");
    return data_[idx];
}

void ArrayD::insert(const std::ptrdiff_t idx, const float val) {
    if (idx < 0 || idx > size_) throw std::out_of_range("ArrayD: invalid insert index");

    std::ptrdiff_t old_size = size_;
    resize(size_ + 1);

    if (idx < old_size) {
        std::memmove(data_ + idx + 1, data_ + idx, (old_size - idx) * sizeof(float));
    }
    data_[idx] = val;
}

void ArrayD::remove(const std::ptrdiff_t idx) {
    if (idx < 0 || idx >= size_) throw std::out_of_range("ArrayD: invalid remove index");

    if (idx < size_ - 1) {
        std::memmove(data_ + idx, data_ + idx + 1, (size_ - idx - 1) * sizeof(float));
    }
    size_--;
}

std::ptrdiff_t ArrayD::find(const float val) const noexcept {
    for (std::ptrdiff_t i = 0; i < size_; ++i) {
        if (data_[i] == val) return i;
    }
    return -1;
}

void ArrayD::sort() noexcept {
    if (size_ > 1) {
        std::sort(data_, data_ + size_);
    }
}
