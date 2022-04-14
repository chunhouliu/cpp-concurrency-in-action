#ifndef THREADSAFE_STACK_H_
#define THREADSAFE_STACK_H_

#include <string>
#include <exception>
#include <stack>
#include <mutex>
#include <memory>

struct empty_stack: public std::exception {
  const char* what() const throw() { return nullptr; }
};

template <typename T>
class threadsafe_stack {
private:
  std::stack<T> data_;
  mutable std::mutex mu_;
public:
  threadsafe_stack() {}
  threadsafe_stack(const threadsafe_stack &rhs) {
    std::lock_guard<std::mutex> lock(rhs.mu_);
    data_ = rhs.data_;
  }
  threadsafe_stack& opeator=(const threadsafe_stack &) =delete;
  void push(T new_value) {
    std::lock_guard<std::mutex> lock(mu_);
    data_.push(new_value);
  }
  void pop(T &result) {
    std::lock_guard<std::mutex> lock(mu_);
    if(data_.empty()) throw empty_stack();
    result = data_.top(); data_.pop();
  }
  std::shared_ptr<T> pop() {
    std::lock_guard<std::mutex> lock(mu_);
    if(data_.empty()) throw empty_stack();
    std::shared_ptr<T> result = std::make_shared<T>(std::move(data_.top()));
    data_.pop();
    return result;
  }
  bool empty() const {
    std::lock_guard<std::mutex> lock(mu_);
    return data_.empty();
  }
};

#endif