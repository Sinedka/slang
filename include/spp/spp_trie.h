#include <array>
#include <cstddef>

constexpr size_t ALPHABET_SIZE = 26;
constexpr size_t MAX_NODES = 128; // максимальное количество узлов
constexpr size_t MAX_WORDS = 28;  // количество слов

struct TrieNode {
  bool is_end = false;
  std::array<int, ALPHABET_SIZE>
      children{}; // индекс в массиве узлов, -1 если нет
  constexpr TrieNode() : is_end(false), children{} {
    for (auto &c : children)
      c = -1;
  }
};

struct Trie {
  std::array<TrieNode, MAX_NODES> nodes{};
  int next_free = 1; // индекс следующего свободного узла

  constexpr Trie() : nodes{}, next_free(1) {}

  constexpr int insert_recursive(int node_idx, const char *str) {
    if (*str == '\0') {
      nodes[node_idx].is_end = true;
      return node_idx;
    }

    int index = *str - 'a';
    if (nodes[node_idx].children[index] == -1) {
      nodes[node_idx].children[index] = next_free++;
    }
    return insert_recursive(nodes[node_idx].children[index], str + 1);
  }

  constexpr void insert(const char *str) { insert_recursive(0, str); }

  bool search_recursive(int node_idx, const char *str) const {
    if (*str == '\0')
      return nodes[node_idx].is_end;
    int index = *str - 'a';
    if (index < 0 || index > 25)
      return false;
    if (nodes[node_idx].children[index] == -1)
      return false;
    return search_recursive(nodes[node_idx].children[index], str + 1);
  }

  bool search(const char *str) const { return search_recursive(0, str); }
};

constexpr std::array<const char *, MAX_WORDS> words = {
    "bool",  "break",   "char",   "const", "continue", "do",     "double",
    "else",  "enum",    "false",  "float", "for",      "if",     "int",
    "long",  "nullptr", "return", "short", "sizeof",   "struct", "ulong",
    "uchar", "uint",    "ushort", "true",  "typedef",  "void",   "while"
};

// Функция build
constexpr Trie build_trie() {
  Trie trie;
  for (auto word : words) {
    trie.insert(word);
  }
  return trie;
}

// Бор, построенный на compile-time
constexpr Trie spp_trie = build_trie();
