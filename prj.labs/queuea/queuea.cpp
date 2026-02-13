#include "queuea.hpp"
#include <stdexcept>
#include <algorithm>

QueueA::QueueA(const QueueA& src) {
    if (src.size_ > 0) {
        reserve(src.size_ + 1); // Fix: +1 для корректного tail
        for (std::size_t i = 0; i < src.size_; ++i) {
            data_[i] = src.data_[(src.head_ + i) % src.capacity_];
        }
        head_ = 0;
        tail_ = src.size_;
        size_ = src.size_;
    }
}

QueueA::~QueueA() {
    delete[] data_;
}

QueueA& QueueA::operator=(const QueueA& src) {
    if (this != &src) {
        clear();
        if (src.size_ > 0) {
            reserve(src.size_ + 1);
            for (std::size_t i = 0; i < src.size_; ++i) {
                data_[i] = src.data_[(src.head_ + i) % src.capacity_];
            }
            head_ = 0;
            tail_ = src.size_;
            size_ = src.size_;
        }
    }
    return *this;
}

bool QueueA::is_empty() const noexcept {
    return size_ == 0;
}

std::size_t QueueA::size() const noexcept {
    return size_;
}

void QueueA::reserve(std::size_t new_cap) {
    if (new_cap <= capacity_) return;

    int64_t* new_data = new int64_t[new_cap];
    for (std::size_t i = 0; i < size_; ++i) {
        new_data[i] = data_[(head_ + i) % capacity_];
    }

    delete[] data_;
    data_ = new_data;
    capacity_ = new_cap;
    head_ = 0;
    tail_ = size_;
}

void QueueA::push(const int64_t val) {
    if (size_ + 1 >= capacity_) { // Fix: условие переполнения
        reserve(capacity_ == 0 ? 2 : capacity_ * 2);
    }
    data_[tail_] = val;
    tail_ = (tail_ + 1) % capacity_;
    size_++;
}

void QueueA::pop() {
    if (is_empty()) throw std::out_of_range("Queue is empty");
    head_ = (head_ + 1) % capacity_;
    size_--;
}

int64_t& QueueA::front() {
    if (is_empty()) throw std::out_of_range("Queue is empty");
    return data_[head_];
}

int64_t QueueA::front() const {
    if (is_empty()) throw std::out_of_range("Queue is empty");
    return data_[head_];
}

void QueueA::clear() noexcept {
    size_ = 0;
    head_ = 0;
    tail_ = 0;
}
