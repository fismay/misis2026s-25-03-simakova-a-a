#include "stacka.hpp"
#include <algorithm>
#include <stdexcept>
StackA::StackA(const StackA& src) {
    if (src.size_ > 0) {
        reserve(src.size_);
        for (std::size_t i = 0; i < src.size_; ++i) data_[i] = src.data_[i];
        size_ = src.size_;
    }
}
StackA::~StackA() { delete[] data_; }
StackA& StackA::operator=(const StackA& src) {
    if (this != &src) {
        clear();
        reserve(src.size_);
        for (std::size_t i = 0; i < src.size_; ++i) data_[i] = src.data_[i];
        size_ = src.size_;
    }
    return *this;
}
bool StackA::is_empty() const noexcept { return size_ == 0; }
std::size_t StackA::size() const noexcept { return size_; }
void StackA::reserve(std::size_t new_cap) {
    if (new_cap <= capacity_) return;
    int64_t* new_data = new int64_t[new_cap];
    for (std::size_t i = 0; i < size_; ++i) new_data[i] = data_[i];
    delete[] data_;
    data_ = new_data;
    capacity_ = new_cap;
}
void StackA::push(const int64_t val) {
    if (size_ == capacity_) reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    data_[size_++] = val;
}
void StackA::pop() {
    if (is_empty()) throw std::out_of_range("Stack is empty");
    size_--;
}
int64_t& StackA::top() {
    if (is_empty()) throw std::out_of_range("Stack is empty");
    return data_[size_ - 1];
}
int64_t StackA::top() const {
    if (is_empty()) throw std::out_of_range("Stack is empty");
    return data_[size_ - 1];
}
void StackA::clear() noexcept { size_ = 0; }
