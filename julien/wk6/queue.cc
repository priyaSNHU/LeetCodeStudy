// Build using:
//   g++ -Wall -Werror --sanitize=address -g -o queue queue.cc && ./queue
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <vector>

template<typename T>
class Queue {
public:
  Queue(size_t min_size)
    : m_read(0), m_wrote(0), m_size(min_size) {
      assert(m_size > 0);
      m_backing = new T[m_size];
    }

  ~Queue() {
    delete [] m_backing;
  }

  bool empty() const { return m_read == m_wrote; }

  T pop() {
    if (empty()) {
      // TODO: Throw!
      return T{};
    }

    const T& res = m_backing[m_read];
    m_read = (m_read + 1) % m_size;
    return res;
  }

  T peek() const {
    if (empty()) {
      // TODO: Throw!
      return T{};
    }

    return m_backing[m_read];
  }

  void push(T t) {
    size_t next = (m_wrote + 1) % m_size;
    if (next == m_read) {
      grow();
    }
    m_backing[m_wrote] = t;
    m_wrote = (m_wrote + 1) % m_size;
  }

  void dump(std::ostream& o) const {
    o << "[";
    size_t toRead = m_read;
    while (toRead != m_wrote) {
      if (toRead != m_read) {
        o << ", ";
      }
      o << m_backing[toRead];
      toRead = (toRead + 1) % m_size;
    }
    o << "]";
  }

private:
  void grow() {
    // TODO: Magic constant.
    size_t new_size = 2 * m_size;
    T* new_backing = new T[new_size];
    size_t toCopy = m_read;
    size_t i = 0;
    while (toCopy != m_wrote) {
      new_backing[i++] = m_backing[toCopy];
      toCopy = (toCopy + 1) % m_size;
    }
    delete [] m_backing;
    m_read = 0;
    m_wrote = i;
    m_backing = new_backing;
    m_size = new_size;
  }

  T* m_backing;
  size_t m_read;
  size_t m_wrote;
  size_t m_size;
};

template <typename U>
std::ostream& operator<<(std::ostream& o, const Queue<U>& b) {
  b.dump(o);
  return o;
}

int main() {
  Queue<int> q(1);
  std::cout << "Empty Queue: " << q << std::endl;
  std::cout << "Queue.empty()? " << q.empty() << std::endl;
  q.push(5);
  std::cout << "Queue after pushing 5: " << q << std::endl;
  std::cout << "Queue.empty()? " << q.empty() << std::endl;
  std::cout << "Queue.peek(): " << q.peek() << std::endl;
  q.push(15);
  q.push(10);
  std::cout << "Queue after pushing 15, 10: " << q << std::endl;
  std::cout << "Queue.empty()? " << q.empty() << std::endl;
  std::cout << "Queue.peek(): " << q.peek() << std::endl;
  int popped = q.pop();
  std::cout << "Popped 5 and got: " << popped << std::endl;
  std::cout << "Queue.empty()? " << q.empty() << std::endl;
  std::cout << "Queue after popped 5: " << q << std::endl;
  std::cout << "Queue.peek() after popped 5: " << q.peek() << std::endl;
  popped = q.pop();
  std::cout << "Popped 15 and got: " << popped << std::endl;
  std::cout << "Queue.empty()? " << q.empty() << std::endl;
  std::cout << "Queue after popped 15: " << q << std::endl;
  std::cout << "Queue.peek() after popped 15: " << q.peek() << std::endl;
  popped = q.pop();
  std::cout << "Popped 10 and got: " << popped << std::endl;
  std::cout << "Queue.empty()? " << q.empty() << std::endl;
  std::cout << "Queue after popped 10: " << q << std::endl;
  std::cout << "Queue.peek() after popped 10: " << q.peek() << std::endl;

  return 0;
}
