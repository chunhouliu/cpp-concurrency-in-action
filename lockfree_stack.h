#ifndef LOCKFREE_STACK_H_
#define LOCKFREE_STACK_H_

#include <atomic>

template <typename T>
class lockfree_stack {
  struct node {
    std::shared_ptr<T> data;
    std::shared_ptr<node> next;
    node(const T& value): data(std::make_shared<T>(value)), next(nullptr) {}
  };
  std::atomic<std::shared_ptr<node>> head;
public:
  lockfree_stack(): head(nullptr) {}
  lockfree_stack(const lockfree_stack &) =delete;
  lockfree_stack& operator=(const lockfree_stack &) =delete;

  void push(T new_value) {
    std::shared_ptr<node> new_node(new node(new_value));
    new_node->next = head.load(std::memory_order_acquire);
    while(!head.compare_exchange_weak(new_node->next, new_node, std::memory_order_acq_rel));
  }

  std::shared_ptr<T> pop() {
    std::shared_ptr<node> old_head = head.load(std::memory_order_acquire);
    while(old_head.get()!=nullptr && !head.compare_exchange_weak(old_head, old_head->next, std::memory_order_acq_rel));
    return old_head.get()!=nullptr ? old_head->data : std::shared_ptr<T>();
  }

};

#endif