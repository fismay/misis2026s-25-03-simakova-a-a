// Стек на массиве: push/pop с увеличением буфера, когда тесно.
#pragma once
#ifndef STACK_STACKA_HPP_67676767
#define STACK_STACKA_HPP_67676767
#include <cstdint>
#include <cstddef>
class StackA {
public:
    StackA() = default;
    StackA(const StackA& src);
    ~StackA();
    StackA& operator=(const StackA& src);
    bool is_empty() const noexcept;
    std::size_t size() const noexcept;
    void push(const int64_t val);
    void pop();
    int64_t& top();
    int64_t top() const;
    void clear() noexcept;
private:
    int64_t* data_ = nullptr;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
    void reserve(std::size_t new_cap);
};
#endif
