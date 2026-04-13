// Очередь на списке: enqueue в хвост, dequeue с головы.
#pragma once
#ifndef QUEUEL_HPP_67676767
#define QUEUEL_HPP_67676767
#include <cstdint>
#include <cstddef>
class QueueL {
public:
    QueueL() = default;
    ~QueueL();
    QueueL(const QueueL&) = delete;
    QueueL& operator=(const QueueL&) = delete;
    bool is_empty() const noexcept;
    size_t size() const noexcept;
    void enqueue(int64_t val);
    void dequeue();
    int64_t& front();
    void clear() noexcept;
private:
    struct Node { int64_t val; Node* next; };
    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    size_t size_ = 0;
};
#endif
