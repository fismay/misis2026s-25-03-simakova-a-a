#pragma once
#ifndef QUEUE_QUEUEA_HPP_2025
#define QUEUE_QUEUEA_HPP_2025

#include <cstdint>
#include <cstddef>

class QueueA {
public:
    QueueA() = default;
    QueueA(const QueueA& src);
    ~QueueA();

    QueueA& operator=(const QueueA& src);

    [[nodiscard]] bool is_empty() const noexcept;
    [[nodiscard]] std::size_t size() const noexcept;

    void push(const int64_t val);
    void pop();
    int64_t& front();
    int64_t front() const;

    void clear() noexcept;

private:
    int64_t* data_ = nullptr;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
    std::size_t head_ = 0;
    std::size_t tail_ = 0;

    void reserve(std::size_t new_cap);
};

#endif
