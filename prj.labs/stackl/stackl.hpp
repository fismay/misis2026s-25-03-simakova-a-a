#pragma once
#ifndef STACKL_HPP
#define STACKL_HPP
#include <cstdint>
#include <cstddef>
#include <initializer_list>
class StackL {
public:
    StackL() = default;
    StackL(std::initializer_list<int64_t> list);
    StackL(const StackL& src);
    ~StackL();
    StackL& operator=(const StackL& src);
    bool is_empty() const noexcept;
    size_t size() const noexcept;
    void push(int64_t val);
    void pop();
    int64_t& top();
    const int64_t& top() const;
    void clear() noexcept;
private:
    struct Node { int64_t val; Node* next; };
    Node* head_ = nullptr;
    size_t size_ = 0;
};
#endif
