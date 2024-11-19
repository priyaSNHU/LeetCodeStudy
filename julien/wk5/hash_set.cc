// Build using:
//   g++ -Wall -Werror --sanitize=address -g -o hash_set hash_set.cc && ./hash_set
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <vector>

// fasthash assumes a 64 bit integer for its bit shifting so we automatically promote our arg.
inline size_t mix_fasthash(uint64_t h) {
    h ^= h >> 23;
    h *= 0x2127599bf4325c37ULL;
    h ^= h >> 47;
    // Truncate the 64 bits to 32 bits if size_t is 32 bits.
    if (sizeof(size_t) == 4) {
      h = (h - (h >> 32)) & ((1LL>>32) - 1);
    }
    return h;
}

template<typename T>
class HashSet {
  // TODO: This won't work for strings.
  static constexpr T kEmptyBucket = std::numeric_limits<T>::min();
  static constexpr double kInitialLoadFactor = 0.8;
  static constexpr int kGrowthMultiplier = 2;

public:
  HashSet(size_t min_size)
    : m_size(std::ceil(min_size / kInitialLoadFactor)) {
      m_buckets = new T[m_size];
      for (int i = 0; i < m_size; ++i) {
        m_buckets[i] = kEmptyBucket;
      }
    }

  ~HashSet() {
    delete [] m_buckets;
  }

  void set(T value) {
    assert(value != kEmptyBucket);

    size_t key = mix_fasthash(value);
    while (m_buckets[key % m_size] != kEmptyBucket) {
      grow();
    }
    m_buckets[key % m_size] = value;
  }

  bool contains(T value) const {
    assert(value != kEmptyBucket);

    size_t key = mix_fasthash(value);
    T t = m_buckets[key % m_size];
    return t != kEmptyBucket && t == value;
  }

  void remove(T value) {
    assert(value != kEmptyBucket);

    size_t key = mix_fasthash(value);
    key %= m_size;
    // TODO: Do defensive programming and do a bucket rather than assert?
    assert(m_buckets[key] == value);
    m_buckets[key] = kEmptyBucket;
  }

  void dump(std::ostream& o) const {
    o << "[";
    bool addComma = false;
    for (int i = 0; i < m_size; ++i) {
      if (m_buckets[i] != kEmptyBucket) {
        if (addComma) {
          o << ", ";
        }
        o << m_buckets[i];
        addComma = true;
      }
    }
    o << "]";
  }

private:
  void grow() {
    int new_size = kGrowthMultiplier * m_size;
    T* new_buckets = new T[new_size];
    for (int i = 0; i < new_size; ++i) {
      new_buckets[i] = kEmptyBucket;
    }

    for (int i = 0; i < m_size; ++i) {
      T value = m_buckets[i];
      if (value == kEmptyBucket) {
        continue;
      }
      size_t key = mix_fasthash(value);
      key %= new_size;
      // TODO: Handle collisions during growth (probably through recursion).
      assert(new_buckets[key] == kEmptyBucket);
      new_buckets[key] = value;
    }

    delete [] m_buckets;
    m_buckets = new_buckets;
    m_size = new_size;
  }

  T* m_buckets;
  int m_size;
};

template <typename U>
std::ostream& operator<<(std::ostream& o, const HashSet<U>& b) {
  b.dump(o);
  return o;
}

int main() {
  HashSet<int> b(1);
  std::cout << "Empty HashSet: " << b << std::endl;
  b.set(5);
  std::cout << "HashSet after setting 5: " << b << std::endl;
  std::cout << "HashSet contains 5? " << b.contains(5) << std::endl;
  std::cout << "HashSet contains 10? " << b.contains(10) << std::endl;
  std::cout << "HashSet contains 15? " << b.contains(15) << std::endl;
  b.set(15);
  b.set(10);
  std::cout << "HashSet after setting 10,15: " << b << std::endl;
  std::cout << "HashSet contains 5? " << b.contains(5) << std::endl;
  std::cout << "HashSet contains 10? " << b.contains(10) << std::endl;
  std::cout << "HashSet contains 15? " << b.contains(15) << std::endl;
  b.remove(5);
  std::cout << "HashSet after removing 5: " << b << std::endl;
  std::cout << "HashSet contains 5? " << b.contains(5) << std::endl;
  std::cout << "HashSet contains 10? " << b.contains(10) << std::endl;
  std::cout << "HashSet contains 15? " << b.contains(15) << std::endl;

  return 0;
}
