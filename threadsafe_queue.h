#ifndef THREADSAFE_QUEUE_H_
#define THREADSAFE_QUEUE_H_

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>

template <typename T>
class threadsafe_queue {
private:
  std::queue<T> data_;
  mutable std::mutex mu_;
  std::condition_variable cv_;
public:
  threadsafe_queue() {}
  threadsafe_queue(const threadsafe_queue &rhs) {
    std::lock_guard<std::mutex> lock(rhs.mu_);
    data_ = rhs.data_;
  }
  threadsafe_queue& operator=(const threadsafe_queue &) =delete;

  void push(T new_value) {
    {
      std::lock_guard<std::mutex> lock(mu_);
      data_.push(new_value);
    }
    cv_.notify_one();
  }

  void wait_and_pop(T& value) {
    std::unique_lock<std::mutex> lock(mu_);
    cv_.wait(lock, [this]{ return !data_.empty(); });
    value=data_.front(); data_.pop();
  }

  std::shared_ptr<T> wait_and_pop() {
    std::unique_lock<std::mutex> lock(mu_);
    cv_.wait(lock, [this]{ return !data_.empty(); });
    std::shared_ptr<T> result = std::make_shared<T>(std::move(data_.front()));
    data_.pop();
    return result;
  }

  bool try_pop(T &value) {
    std::lock_guard<std::mutex> lock(mu_);
    if(data_.empty()) return false;
    value = data_.front(); data_.pop();
    return true;
  }

  std::shared_ptr<T> try_pop() {
    std::lock_guard<std::mutex> lock(mu_);
    if(data_.empty()) return std::shared_ptr<T>();
    std::shared_ptr<T> result = std::make_shared<T>(std::move(data_.front()));
    data_.pop();
    return result;
  }

  bool empty() const {
    std::lock_guard<std::mutex> lock(mu_);
    return data_.empty();
  }

};

#endif