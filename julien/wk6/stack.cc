// Build using:
//   g++ -Wall -Werror --sanitize=address -g -o stack stack.cc && ./stack
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <vector>

template<typename T>
class Stack {
public:
  Stack(size_t min_size)
    : m_used(0), m_size(min_size) {
      assert(m_size > 0);
      m_backing = new T[m_size];
    }

  ~Stack() {
    delete [] m_backing;
  }

  bool empty() const { return m_used <= 0; }

  T pop() {
    if (empty()) {
      // TODO: Throw!
      return T{};
    }

    return m_backing[--m_used];
  }

  T peek() const {
    if (empty()) {
      // TODO: Throw!
      return T{};
    }

    return m_backing[m_used - 1];
  }

  void push(T t) {
    if (m_used >= m_size) {
      grow();
    }
    m_backing[m_used++] = t;
  }

  void dump(std::ostream& o) const {
    o << "[";
    for (size_t i = 0; i < m_used; ++i) {
      if (i != 0) {
        o << ", ";
      }
      o << m_backing[i];
    }
    o << "]";
  }

private:
  void grow() {
    // TODO: Magic constant.
    size_t new_size = 2 * m_size;
    // TODO: Use memset.
    T* new_backing = new T[new_size];
    for (size_t i = 0; i < m_size; ++i) {
      new_backing[i] = m_backing[i];
    }
    delete [] m_backing;
    m_backing = new_backing;
    m_size = new_size;
  }

  T* m_backing;
  size_t m_used;
  size_t m_size;
};

template <typename U>
std::ostream& operator<<(std::ostream& o, const Stack<U>& b) {
  b.dump(o);
  return o;
}

int main() {
  Stack<int> b(1);
  std::cout << "Empty Stack: " << b << std::endl;
  std::cout << "Stack.empty()? " << b.empty() << std::endl;
  b.push(5);
  std::cout << "Stack after pushing 5: " << b << std::endl;
  std::cout << "Stack.empty()? " << b.empty() << std::endl;
  std::cout << "Stack.peek(): " << b.peek() << std::endl;
  b.push(15);
  b.push(10);
  std::cout << "Stack after pushing 15, 10: " << b << std::endl;
  std::cout << "Stack.empty()? " << b.empty() << std::endl;
  std::cout << "Stack.peek(): " << b.peek() << std::endl;
  int popped = b.pop();
  std::cout << "Popped 10 and got: " << popped << std::endl;
  std::cout << "Stack.empty()? " << b.empty() << std::endl;
  std::cout << "Stack after popped 10: " << b << std::endl;
  std::cout << "Stack.peek() after popped 10: " << b.peek() << std::endl;
  popped = b.pop();
  std::cout << "Popped 15 and got: " << popped << std::endl;
  std::cout << "Stack.empty()? " << b.empty() << std::endl;
  std::cout << "Stack after popped 15: " << b << std::endl;
  std::cout << "Stack.peek() after popped 15: " << b.peek() << std::endl;
  popped = b.pop();
  std::cout << "Popped 5 and got: " << popped << std::endl;
  std::cout << "Stack.empty()? " << b.empty() << std::endl;
  std::cout << "Stack after popped 15: " << b << std::endl;
  std::cout << "Stack.peek() after popped 15: " << b.peek() << std::endl;

  return 0;
}
