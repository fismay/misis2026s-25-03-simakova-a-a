#include "queuel.hpp"
#include <stdexcept>
QueueL::~QueueL() { clear(); }
bool QueueL::is_empty() const noexcept { return head_ == nullptr; }
size_t QueueL::size() const noexcept { return size_; }
void QueueL::enqueue(int64_t val) {
    Node* new_node = new Node{ val, nullptr };
    if (is_empty()) { head_ = tail_ = new_node; }
    else { tail_->next = new_node; tail_ = new_node; }
    size_++;
}
void QueueL::dequeue() {
    if (is_empty()) return;
    Node* to_delete = head_;
    head_ = head_->next;
    delete to_delete;
    size_--;
    if (head_ == nullptr) tail_ = nullptr;
}
int64_t& QueueL::front() {
    if (is_empty()) throw std::runtime_error("Queue is empty");
    return head_->val;
}
void QueueL::clear() noexcept { while (!is_empty()) dequeue(); }
