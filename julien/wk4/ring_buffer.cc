// Build using:
//   g++ -Wall -Werror --sanitize=address -g -o ring_buffer ring_buffer.cc && ./ring_buffer
#include <cassert>
#include <iostream>
#include <vector>

template<typename T>
class RingBuffer {
public:
  // Note: Only size-1 element can be stored in the buffer.
  RingBuffer(size_t size)
    : m_buf(new T[size])
    , m_size(size)
    , m_readIdx(0)
    , m_writeIdx(0) {}

  ~RingBuffer() {
    delete [] m_buf;
  }

  // TODO: We could allow resizing, should we?
  size_t size() const { return m_size; }
  size_t length() const { return m_size; }

  // TODO: This is logically const, make it so?
  T readOne(bool& read) {
    if (m_readIdx == m_writeIdx) {
      read = false;
      return T{};
    }
    read =  true;
    T res = m_buf[m_readIdx];
    m_readIdx = (m_readIdx + 1) % m_size;
    return res;
  }

  // TODO: We should allow a reference/move here instead of doing a copy.
  bool writeOne(T data) {
    size_t nextWriteIdx = (m_writeIdx + 1) % m_size;
    if (nextWriteIdx == m_readIdx) {
      return false;
    }
    m_buf[m_writeIdx] = data;
    m_writeIdx = nextWriteIdx;
    return true;
  }

  size_t write(const std::vector<T>& v) {
    size_t vIdx = 0;
    while (vIdx < v.size()) {
      size_t nextWriteIdx = (m_writeIdx + 1) % m_size;
      if (nextWriteIdx == m_readIdx) {
        // Prevent overflow.
        return vIdx;
      }
      m_buf[m_writeIdx] = v[vIdx];
      vIdx++;
      m_writeIdx = nextWriteIdx;
    }
    return vIdx;
  }

  void dump(std::ostream& o) const {
    o << "[";
    for (size_t idx = m_readIdx; idx != m_writeIdx; idx = (idx + 1) % m_size) {
      if (idx != m_readIdx) {
        o << ", ";
      }
      o << m_buf[idx];
    }
    o << "] (readIdx=";
    o << m_readIdx;
    o << ", writeIdx=";
    o << m_writeIdx;
    o << ")";
  }

private:
  T* m_buf;
  size_t m_size;
  size_t m_readIdx;
  size_t m_writeIdx;
};

template <typename U>
std::ostream& operator<<(std::ostream& o, const RingBuffer<U>& b) {
  b.dump(o);
  return o;
}

int main() {
  RingBuffer<int> b(5);
  std::cout << "Empty ringbuffer: " << b << std::endl;
  bool read;
  b.readOne(read);
  std::cout << "Empty ringbuffer after reading: " << b << "(read=" << read << ")" << std::endl;
  bool written = b.writeOne(1);
  std::cout << "Ringbuffer after append 1: " << b << "(written=" << written << ")" << std::endl;
  written = b.writeOne(1);
  std::cout << "Ringbuffer after append 1 (again): " << b << "(written=" << written << ")" << std::endl;
  b.readOne(read);
  std::cout << "Ringbuffer after reading 1: " << b << "(read=" << read << ")" << std::endl;
  size_t elemWritten = b.write(std::vector<int>{2,3,4,5,6,7});
  std::cout << "Ringbuffer after appending multiple: " << b << "(elemWritten=" << elemWritten << ")" << std::endl;
  written = b.writeOne(1);
  std::cout << "Ringbuffer after trying to append to full ringbuffer: " << b << "(written=" << written << ")" << std::endl;

  return 0;
}
