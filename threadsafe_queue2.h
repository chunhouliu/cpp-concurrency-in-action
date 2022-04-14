#ifndef THREADSAFE_QUEUE_H_
#define THREADSAFE_QUEUE_H_

#include <memory>
#include <mutex>
#include <condition_variable>
#include <queue>

template <typename T>
class threadsafe_queue {
private:
  std::queue<std::shared_ptr<T>> data_;
  mutable std::mutex mu_;
  std::condition_variable cv_;
public:
  threadsafe_queue() {}
  
  void push(T new_value) {
    std::shared_ptr<T> d = std::make_shared(std::move(new_value));
    std::lock_guard<std::mutex> lock(mu_);
    data_.push(d);
    cv_.notify_one();
  }

  void wait_and_pop(T& value) {
    std::unique_lock<std::mutex> lock(mu_);
    cv_.wait(lock, [this]{ return !data_.empty(); });
    value=std::move(*data_.front()); data_.pop();
  }

  std::shared_ptr<T> wait_and_pop() {
    std::unique_lock<std::mutex> lock(mu_);
    cv_.wait(lock, [this]{ return !data_.empty(); });
    std::shared_ptr<T> res = data_.front();
    data_.pop();
    return res;
  }

  bool try_pop(T& value) {
    std::lock_guard<std::mutex> lock(mu_);
    if(data_.empty()) return false;
    value = std::move(*data_.front());
    data_.pop();
    return true;
  }

  std::shared_ptr<T> try_pop() {
    std::lock_guard<std::mutex> lock(mu_);
    if(data_.empty()) return std::shared_ptr<T>();
    std::shared_ptr<T> res = data_.front();
    data_.pop();
    return res;
  }

  bool empty() const {
    std::lock_guard<std::mutex> lock(mu_);
    return data_.empty();
  }
};

#endif