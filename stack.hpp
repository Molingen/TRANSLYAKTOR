#pragma once
#include <vector>
#include <stdexcept>

namespace ds {

template <typename T, template<typename...> class TContainer = std::vector>
class Stack {
public:
    Stack() = default;
    void push(const T& el) { cont_.push_back(el); }
    void push(T&& el) { cont_.push_back(std::move(el)); }
    void pop() {
        if (cont_.empty()) throw std::out_of_range("Stack::pop on empty stack");
        cont_.pop_back();
    }
    const T& top() const {
        if (cont_.empty()) throw std::out_of_range("Stack::top on empty stack");
        return cont_.back();
    }
    T& top() {
        if (cont_.empty()) throw std::out_of_range("Stack::top on empty stack");
        return cont_.back();
    }
    size_t size() const noexcept { return cont_.size(); }
    bool empty() const noexcept { return cont_.empty(); }
    void clear() { cont_.clear(); }

private:

    TContainer<T> cont_;

};

} // namespace ds
