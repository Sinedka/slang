#include <algorithm>
#include <array>
#include <span>
#include <vector>

enum class TokenType : char {
  IDENTIFIERS,
  KEYWORDS,
  LITERALS,
  OPERATORS,
  SEPARATORS,
  TEOF,
};

struct Entry {
  const char *key;
  TokenType value;
};

constexpr auto tokenList = std::to_array<Entry>({{"+", TokenType::OPERATORS},
                                                 {"-", TokenType::OPERATORS},
                                                 {"--", TokenType::OPERATORS},
                                                 {"+=", TokenType::OPERATORS},
                                                 {"++", TokenType::OPERATORS},
                                                 {"*", TokenType::OPERATORS}});

constexpr auto t = std::to_array<int>({3, 1, 3, 4, 5});

consteval std::size_t strlen_cx(const char *str) {
  std::size_t len = 0;
  while (str[len] != '\0')
    ++len;
  return len;
}

consteval bool compare(Entry A, Entry B) {
  return strlen_cx(A.key) > strlen_cx(B.key);
}

consteval size_t findmaxl(const auto &arr) {
  size_t maxl = 0;

  for (int i = 0; i < arr.size(); i++)
    maxl = std::max(maxl, strlen_cx(arr[i].key));

  return maxl;
}

constexpr auto maxl = findmaxl(tokenList);

consteval std::array<size_t, maxl> findlforl(const auto &arr) {
  std::array<size_t, maxl> ans;
  ans.fill(0);
  for (size_t i = 0; i < maxl; i++) {
    for (auto x : arr) {
      if (strlen_cx(x.key) == i)
        ans[i]++;
    }
  }
  return ans;
}

consteval auto sort(const auto &arr) {
  auto result = arr;
  std::sort(begin(result), end(result), compare) ;
  return result;
}

constexpr std::array<std::span<const Entry>, maxl> make_views() {
  std::array<std::span<const Entry>, maxl> result;


}

constexpr auto tokenListSorted = sort(tokenList);
