// Build using:
//   g++ -Wall -Werror --sanitize=address -g -o priority_queue priority_queue.cc && ./priority_queue
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <vector>

template<typename T>
struct Node {
  int priority;
  T t;
};

template<typename T>
class PriorityQueue {
public:
  PriorityQueue(size_t min_size)
    : m_used(0), m_size(min_size) {
      assert(m_size > 0);
      m_backing = new Node<T>[m_size];
    }

  ~PriorityQueue() {
    delete [] m_backing;
  }

  bool empty() const { return m_used == 0; }

  T pop() {
    if (empty()) {
      // TODO: Throw!
      return T{};
    }

    Node<T> res = m_backing[0];
    m_used--;
    if (m_used > 0) {
      m_backing[0] = m_backing[m_used];
      updateRecursive(0);
    }
    return res.t;
  }

  T peek() const {
    if (empty()) {
      // TODO: Throw!
      return T{};
    }

    return m_backing[0].t;
  }

  void push(int priority, T t) {
    if (m_used == m_size) {
      grow();
    }
    int idx = m_used++;
    m_backing[idx] = Node<T>{priority, t};
    reshapeHeapFrom(idx);
  }

  void dump(std::ostream& o) const {
    o << "[";
    for (size_t i = 0; i < m_used; ++i) {
      if (i > 0) {
        o << ", ";
      }
      o << m_backing[i].t;
    }
    o << "]";
  }

private:
  void updateRecursive(size_t idx) {
    assert(idx < m_used);

    size_t left = 2 * idx + 1;
    if (left < m_used && m_backing[left].priority >= m_backing[idx].priority) {
      Node<T> tmp = m_backing[left];
      m_backing[left] = m_backing[idx];
      m_backing[idx] = tmp;
      updateRecursive(left);
    }

    size_t right = 2 * idx + 2;
    if (right < m_used && m_backing[right].priority >= m_backing[idx].priority) {
      Node<T> tmp = m_backing[right];
      m_backing[right] = m_backing[idx];
      m_backing[idx] = tmp;
      updateRecursive(right);
    }
  }

  void reshapeHeapFrom(size_t idx) {
    if (idx == 0) {
      return;
    }

    size_t parent = floor((idx - 1.0) / 2.0);
    if (m_backing[parent].priority < m_backing[idx].priority) {
      Node<T> tmp = m_backing[parent];
      m_backing[parent] = m_backing[idx];
      m_backing[idx] = tmp;
      reshapeHeapFrom(parent);
    }
  }

  void grow() {
    // TODO: Magic constant.
    size_t new_size = 2 * m_size;
    // TODO: Use memset.
    Node<T>* new_backing = new Node<T>[new_size];
    for (size_t i = 0; i < m_used; ++i) {
      new_backing[i] = m_backing[i];
    }
    delete [] m_backing;
    m_backing = new_backing;
    m_size = new_size;
  }

  Node<T>* m_backing;
  size_t m_used;
  size_t m_size;
};

template <typename U>
std::ostream& operator<<(std::ostream& o, const PriorityQueue<U>& b) {
  b.dump(o);
  return o;
}

int main() {
  PriorityQueue<int> q(1);
  std::cout << "Empty PriorityQueue: " << q << std::endl;
  std::cout << "PriorityQueue.empty()? " << q.empty() << std::endl;
  q.push(10, 5);
  std::cout << "PriorityQueue after pushing 5: " << q << std::endl;
  std::cout << "PriorityQueue.empty()? " << q.empty() << std::endl;
  std::cout << "PriorityQueue.peek(): " << q.peek() << std::endl;
  q.push(20, 15);
  q.push(1, 10);
  std::cout << "PriorityQueue after pushing 15, 10: " << q << std::endl;
  std::cout << "PriorityQueue.empty()? " << q.empty() << std::endl;
  std::cout << "PriorityQueue.peek(): " << q.peek() << std::endl;
  int popped = q.pop();
  std::cout << "Popped 15 and got: " << popped << std::endl;
  std::cout << "PriorityQueue.empty()? " << q.empty() << std::endl;
  std::cout << "PriorityQueue after popped 15: " << q << std::endl;
  std::cout << "PriorityQueue.peek() after popped 15: " << q.peek() << std::endl;
  popped = q.pop();
  std::cout << "Popped 5 and got: " << popped << std::endl;
  std::cout << "PriorityQueue.empty()? " << q.empty() << std::endl;
  std::cout << "PriorityQueue after popped 5: " << q << std::endl;
  std::cout << "PriorityQueue.peek() after popped 5: " << q.peek() << std::endl;
  popped = q.pop();
  std::cout << "Popped 10 and got: " << popped << std::endl;
  std::cout << "PriorityQueue.empty()? " << q.empty() << std::endl;
  std::cout << "PriorityQueue after popped 10: " << q << std::endl;
  std::cout << "PriorityQueue.peek() after popped 10: " << q.peek() << std::endl;

  return 0;
}
