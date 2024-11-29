// Build using:
//   g++ -Wall -Werror --sanitize=address -g -o decode_string decode_string.cc && ./decode_string
#include <cassert>
#include <iostream>
#include <vector>

// TODO: Use the STL variant instead of reimplementing them.
static bool isDigit(char c) {
    return c >= '0' && c <= '9';
}
static bool isEnglish(char c) {
    return c >= 'a' && c <= 'z';
}

// Node is either a string or an int depending on |isRepeat|.
struct Node {
  bool isRepeat;
  // TODO: Figure out how to use a union to save on memory.
  // TODO: Also repeat can fit into log(300)/log(2) == 9 bits so we could save some memory.
  int repeat;
  std::string s;
};

class CustomStack {
public:
  CustomStack() {}

  void pushRepeat(int repeat) {
    m_repeatIdx.push_back(m_backing.size());
    m_backing.push_back(Node{true, repeat, std::string{}});
  }

  void pushStr(const std::string& s) {
    m_backing.push_back(Node{false, 0, s});
  }

  void resolve() {
    assert(!m_repeatIdx.empty());
    size_t lastIdx = m_repeatIdx.back();
    m_repeatIdx.pop_back();
    std::string repeatStr;
    for (size_t i = lastIdx + 1; i < m_backing.size(); ++i) {
      Node& n = m_backing[i];
      assert(n.isRepeat == false);
      repeatStr += n.s;
    }

    assert(m_backing[lastIdx].isRepeat);
    std::string res = repeatStr;
    for (int i = 1; i < m_backing[lastIdx].repeat; ++i) {
      res += repeatStr;
    }
    // Overwrite the lastIdx and clear follow-up repeat.
    m_backing[lastIdx].isRepeat = false;
    m_backing[lastIdx].s = res;
    m_backing.erase(m_backing.begin() + lastIdx + 1, m_backing.end());
  }

  std::string getAnswer() const {
    assert(m_repeatIdx.empty());
    assert(!m_backing.empty());
    std::string s;
    for (size_t i = 0; i < m_backing.size(); ++i) {
      assert(m_backing[i].isRepeat == false);
      s += m_backing[i].s;
    }
    return s;
  }

private:
  std::vector<size_t> m_repeatIdx;
  std::vector<Node> m_backing;
};

class StackSolution {
public:
    static std::string decodeString(const std::string& s) {
      CustomStack st;
      size_t idx = 0;
      while (idx < s.size()) {
        if (isDigit(s[idx])) {
          int repeat = 0;
          while (isDigit(s[idx])) {
              repeat *= 10;
              repeat += s[idx] - '0';
              ++idx;
          }
          assert(s[idx] == '[');
          ++idx;
          st.pushRepeat(repeat);
          continue;
        }
        if (isEnglish(s[idx])) {
          int endIdx = idx + 1;
          while (isEnglish(s[endIdx])) {
            ++endIdx;
          }
          st.pushStr(s.substr(idx, endIdx - idx));
          idx = endIdx;
          continue;
        }
        if (s[idx] == '[') {
          // Nothing to do.
          continue;
        }
        assert(s[idx] == ']');
        ++idx;
        st.resolve();
      }

      return st.getAnswer();
    }
};

class RecursiveSolution {
    static std::string decodeStringRecursive(std::string_view s) {
        if (s.empty()) {
            return std::string{};
        }
        std::string res;
        size_t idx = 0;
        while (idx < s.length()) {
            if (isEnglish(s[idx])) {
                res += s[idx];
                ++idx;
                continue;
            }
            if (isDigit(s[idx])) {
                int repeat = 0;
                while (isDigit(s[idx])) {
                    repeat *= 10;
                    repeat += s[idx] - '0';
                    ++idx;
                }
                assert(s[idx] == '[');
                ++idx;
                int level = 1;
                size_t endIdx = idx + 1;
                while (endIdx < s.length()) {
                    if (s[endIdx] == '[') {
                        level++;
                    } else if (s[endIdx] == ']') {
                        level--;
                        if (level == 0) {
                            break;
                        }
                    }
                    ++endIdx;
                }
                std::string repeatStr = decodeStringRecursive(s.substr(idx, endIdx - idx));
                for (int i = 0; i < repeat; ++i) {
                    res += repeatStr;
                }
                idx = endIdx + 1;
                continue;
            }
            // Ignore ']'.
            assert(s[idx] == ']');
            ++idx;
        }

        return res;
    }
public:
    static std::string decodeString(const std::string& s) {
        // Understand:
        // - valid s .
        // - allow recursive def.
        // - only lowerscase English repeated.
        // - corner case:
        // - 10[] --> output: ""

        // Match:
        // Recursion --> need to handle nested def.
        // Parsing --> can parse/split input first then process (heavy handed methinks)
        // Reverse Polish notation --> use a stack?


        return decodeStringRecursive(std::string_view{s});
    }
};

void test(const std::string& s, const std::string& expected) {
  std::cout << "Testing (recursive) \"" << s << "\" ... ";
  std::string res = RecursiveSolution::decodeString(s);
  if (res != expected) {
    std::cout << "!!!! FAILED, got: \"" << res << "\", but expected: \"" << expected << "\"" << std::endl;
  } else {
    std::cout << "PASSED (expected: \"" << expected << "\")" << std::endl;
  }

  std::cout << "Testing (stack) \"" << s << "\" ... ";
  res = StackSolution::decodeString(s);
  if (res != expected) {
    std::cout << "!!!! FAILED, got: \"" << res << "\", but expected: \"" << expected << "\"" << std::endl;
  } else {
    std::cout << "PASSED (expected: \"" << expected << "\")" << std::endl;
  }
}

int main() {
  test("3[a]2[bc]", "aaabcbc");
  test("3[a2[c]]", "accaccacc");
  test("2[abc]3[cd]ef", "abcabccdcdcdef");
}
