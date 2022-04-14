#ifndef QUEUE_H_
#define QUEUE_H_

#include <memory>

template <typename T>
class queue {
  struct node {
    T data;
    std::unique_ptr<node> next;
    node(T d): data(std::move(d)) {}
  };

  std::unique_ptr<node> head;
  node* tail;
public:
  queue(){}
  queue(const queue &) =delete;
  queue& operator=(const queue &) =delete;

  std::shared_ptr<T> try_pop() {
    if(head.get() == nullptr) return std::shared_ptr<T>();
    std::shared_ptr<T> result = std::make_shared<T>(std::move(head->data));
    head=head->next;
    return result;
  }

  void push(T new_value) {
    std::unique_ptr<node> p(new node(new_value));
    node* new_tail = tail;
    if(tail) {
      tail->next = std::move(p);
    } else {
      head = std::move(p);
    }
    tail = new_tail;
  }
};

#endif