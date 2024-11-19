// Build using:
//   g++ -Wall -Werror --sanitize=address -g -o hash_table hash_table.cc && ./hash_table
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
class HashTable {
  static constexpr double kInitialLoadFactor = 0.8;
  static constexpr int kGrowthMultiplier = 2;

  struct Entry {
    size_t key;
    T val;
  };
public:
  HashTable(size_t min_size)
    : m_size(std::ceil(min_size / kInitialLoadFactor)) {
      m_buckets = new Entry*[m_size];
      for (size_t i = 0; i < m_size; ++i) {
        m_buckets[i] = nullptr;
      }
    }

  ~HashTable() {
    for (size_t i = 0; i < m_size; ++i) {
      if (m_buckets[i]) {
        delete m_buckets[i];
      }
    }

    delete [] m_buckets;
  }

  void set(size_t k, T value) {
    size_t key = mix_fasthash(k);
    while (m_buckets[key % m_size] != nullptr) {
      grow();
    }
    m_buckets[key % m_size] = new Entry{k, value};
  }

  bool contains(size_t k) const {
    size_t key = mix_fasthash(k);
    key %= m_size;
    Entry* e = m_buckets[key % m_size];
    return e && e->key == k;
  }

  void remove(size_t k) {
    size_t key = mix_fasthash(k);
    key %= m_size;
    if (m_buckets[key] && m_buckets[key]->key == k) {
      delete m_buckets[key];
      m_buckets[key] = nullptr;
    }
  }

  void dump(std::ostream& o) const {
    o << "[";
    bool addComma = false;
    for (size_t i = 0; i < m_size; ++i) {
      Entry* e = m_buckets[i];
      if (e) {
        if (addComma) {
          o << ", ";
        }
        o << e->key << ": " << e->val;
        addComma = true;
      }
    }
    o << "]";
  }

private:
  void grow() {
    size_t new_size = kGrowthMultiplier * m_size;
    Entry** new_buckets = new Entry*[new_size];
    for (size_t i = 0; i < new_size; ++i) {
      new_buckets[i] = nullptr;
    }

    for (size_t i = 0; i < m_size; ++i) {
      Entry* e = m_buckets[i];
      if (e == nullptr) {
        continue;
      }
      size_t key = mix_fasthash(e->key);
      key %= new_size;
      // TODO: Handle collisions during growth (probably through recursion).
      assert(new_buckets[key] == nullptr);
      new_buckets[key] = e;
    }

    delete [] m_buckets;
    m_buckets = new_buckets;
    m_size = new_size;
  }

  // TODO: Use std::unique_ptr.
  Entry** m_buckets;
  size_t m_size;
};

template <typename U>
std::ostream& operator<<(std::ostream& o, const HashTable<U>& b) {
  b.dump(o);
  return o;
}

int main() {
  HashTable<int> b(1);
  std::cout << "Empty HashSet: " << b << std::endl;
  b.set(5, 1245);
  std::cout << "HashSet after setting 5: " << b << std::endl;
  std::cout << "HashSet contains 5? " << b.contains(5) << std::endl;
  std::cout << "HashSet contains 10? " << b.contains(10) << std::endl;
  std::cout << "HashSet contains 15? " << b.contains(15) << std::endl;
  b.set(15, 4567);
  b.set(10, 8790);
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
