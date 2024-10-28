#include <cassert>
#include <iostream>

template<typename T>
class NaiveVector {
  public:
    NaiveVector(size_t size, const T& t) {
      m_backing = (T*)::malloc(size * sizeof(T));
      m_size = size;
      for (size_t i = 0; i < size; ++i) {
        m_backing[i] = t;
      }
    }

    ~NaiveVector() {
      ::free(m_backing);
      m_backing = nullptr;
      m_size = 0;
    }

    T& at(size_t i) const {
      checkPos(i);

      return m_backing[i];
    }

    T& operator[](size_t i) const {
      return this->at(i);
    }

    // Use operator==(const T&).
    // Returns -1 if not found.
    size_t find(const T& val) const {
      for (size_t i = 0; i < m_size; ++i) {
        if (m_backing[i] == val) {
          return i;
        }
      }
      // TODO: I don't think this works great as size_t is unsigned.
      return -1;
    }

    void append(T val) {
      insert(m_size, val);
    }

    void insert(size_t pos, T val) {
      assert(pos >= 0);
      assert(pos <= m_size);

      // TODO: Grow efficiently.
      m_backing = (T*)::realloc(m_backing, (m_size + 1) * sizeof(T));
      for (size_t i = pos; i < m_size; ++i) {
        m_backing[i + 1] = m_backing[i];
      }
      m_backing[pos] = val;
      m_size += 1;
    }

    void remove(size_t pos) {
      checkPos(pos);
      // TODO: Reuse existing capacity.
      T* new_backing = (T*)::malloc((m_size - 1) * sizeof(T));
      for (size_t i = 0; i < pos; ++i) {
        new_backing[i] = m_backing[i];
      }
      for (size_t i = pos; i < m_size - 1; ++i) {
        new_backing[i] = m_backing[i + 1];
      }
      ::free(m_backing);
      m_size -= 1;
      m_backing = new_backing;
    }

    size_t size() const { return m_size; }

    // TODO: Support resizing.

  private:
    NaiveVector() = delete;
    // TODO: Make noncopyable for now.
    template<typename U>
    NaiveVector(const NaiveVector<U>&) = delete;
    template<typename U>
    void operator=(const NaiveVector<U>&) = delete;

    // TODO: Make container moveable.

    inline void checkPos(size_t pos) const {
      assert(pos >= 0);
      assert(pos < m_size);
    }
    T* m_backing;
    size_t m_size;
};

template<typename T>
std::ostream& operator<<(std::ostream& o, const NaiveVector<T>& v) {
  o << "[";
  for (size_t i = 0; i < v.size(); ++i) {
    if (i > 0) {
      o << ", ";
    }
    o << v[i];
  }
  o << "]";

  return o;
}


template<typename T>
class BetterVector {
  public:
    BetterVector(size_t size, const T& t) {
      m_capacity = 0;
      m_backing = nullptr;
      growIfNeeded(size);
      m_size = size;
      for (size_t i = 0; i < size; ++i) {
        m_backing[i] = t;
      }
    }

    ~BetterVector() {
      ::free(m_backing);
      m_backing = nullptr;
      m_size = 0;
    }

    T& at(size_t i) const {
      checkPos(i);

      return m_backing[i];
    }

    T& operator[](size_t i) const {
      return this->at(i);
    }

    // Use operator==(const T&).
    // Returns -1 if not found.
    size_t find(const T& val) const {
      for (size_t i = 0; i < m_size; ++i) {
        if (m_backing[i] == val) {
          return i;
        }
      }
      // TODO: I don't think this works great as size_t is unsigned.
      return -1;
    }

    void append(T val) {
      insert(m_size, val);
    }

    void insert(size_t pos, T val) {
      assert(pos >= 0);
      assert(pos <= m_size);

      growIfNeeded(m_size + 1);
      for (size_t i = pos; i < m_size; ++i) {
        m_backing[i + 1] = m_backing[i];
      }
      m_backing[pos] = val;
      m_size += 1;
    }

    void remove(size_t pos) {
      checkPos(pos);
      for (size_t i = pos; i < m_size - 1; ++i) {
        m_backing[i] = m_backing[i + 1];
      }
      m_size -= 1;
    }

    size_t size() const { return m_size; }

    void resize(size_t newCapacity) { growIfNeeded(newCapacity); }
    // TODO: Add shrink to fit?

  private:
    BetterVector() = delete;
    // TODO: Make noncopyable for now.
    template<typename U>
    BetterVector(const BetterVector<U>&) = delete;
    template<typename U>
    void operator=(const BetterVector<U>&) = delete;

    // TODO: Make container moveable.

    void growIfNeeded(size_t newCapacity) {
      if (m_capacity < newCapacity) {
        m_capacity = 1.4 * newCapacity;
        m_backing = (T*)::realloc(m_backing, m_capacity * sizeof(T));
      }
    }

    inline void checkPos(size_t pos) const {
      assert(pos >= 0);
      assert(pos < m_size);
    }

    T* m_backing;
    size_t m_size;
    size_t m_capacity;
};

template<typename T>
std::ostream& operator<<(std::ostream& o, const BetterVector<T>& v) {
  o << "[";
  for (size_t i = 0; i < v.size(); ++i) {
    if (i > 0) {
      o << ", ";
    }
    o << v[i];
  }
  o << "]";

  return o;
}
int main() {
  {
    std::cout << "NaiveVector" << std::endl;
    NaiveVector<int> v(1, 10);
    std::cout << "Default vector: " << v << std::endl;
    v.insert(0, 12);
    std::cout << "After add(): " << v << std::endl;
    v.append(14);
    std::cout << "After append(): " << v << std::endl;
    v.remove(0);
    std::cout << "After remove(): " << v << std::endl;
    v.remove(v.size() - 1);
    std::cout << "After remove(end): " << v << std::endl;
    std::cout << "NaiveVector ==== DONE" << std::endl;
  }

  {
    std::cout << "BetterVector" << std::endl;
    BetterVector<int> v(1, 10);
    std::cout << "Default vector: " << v << std::endl;
    v.insert(0, 12);
    std::cout << "After add(): " << v << std::endl;
    v.append(14);
    std::cout << "After append(): " << v << std::endl;
    v.remove(0);
    std::cout << "After remove(): " << v << std::endl;
    v.remove(v.size() - 1);
    std::cout << "After remove(end): " << v << std::endl;
    std::cout << "BetterVector ==== DONE" << std::endl;
  }

  return 0;
}
