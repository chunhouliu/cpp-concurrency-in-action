#ifndef SPINLOCK_MUTEX_H_
#define SPINLOCK_MUTEX_H_

#include <atomic>

class spinlock_mutex {
  std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:
  spinlock_mutex(){}

  void lock() {
    while(flag.test_and_set(std::memory_order_acquire)) ;
  }

  void unlock() {
    flag.clear(std::memory_order_release);
  }
};

#endif