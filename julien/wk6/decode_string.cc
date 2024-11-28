// Build using:
//   g++ -Wall -Werror --sanitize=address -g -o decode_string decode_string.cc && ./decode_string
#include <cassert>
#include <iostream>

class Solution {
    static bool isDigit(char c) {
        return c >= '0' && c <= '9';
    }
    static bool isEnglish(char c) {
        return c >= 'a' && c <= 'z';
    }

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
  std::cout << "Testing \"" << s << "\" ... ";
  std::string res = Solution::decodeString(s);
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
