#include "stackl.hpp"
#include <stdexcept>
#include <algorithm>
StackL::StackL(std::initializer_list<int64_t> list) {
    for (auto it = list.begin(); it != list.end(); ++it) push(*it);
}
StackL::StackL(const StackL& src) {
    if (src.head_) {
        Node* src_curr = src.head_;
        Node** dst_curr = &head_;
        while (src_curr) {
            *dst_curr = new Node{ src_curr->val, nullptr };
            dst_curr = &((*dst_curr)->next);
            src_curr = src_curr->next;
        }
        size_ = src.size_;
    }
}
StackL::~StackL() { clear(); }
StackL& StackL::operator=(const StackL& src) {
    if (this != &src) {
        StackL temp(src);
        std::swap(head_, temp.head_);
        std::swap(size_, temp.size_);
    }
    return *this;
}
bool StackL::is_empty() const noexcept { return head_ == nullptr; }
size_t StackL::size() const noexcept { return size_; }
void StackL::push(int64_t val) {
    head_ = new Node{ val, head_ };
    size_++;
}
void StackL::pop() {
    if (is_empty()) return;
    Node* to_delete = head_;
    head_ = head_->next;
    delete to_delete;
    size_--;
}
int64_t& StackL::top() {
    if (is_empty()) throw std::runtime_error("Stack is empty");
    return head_->val;
}
const int64_t& StackL::top() const {
    if (is_empty()) throw std::runtime_error("Stack is empty");
    return head_->val;
}
void StackL::clear() noexcept { while (!is_empty()) pop(); }
