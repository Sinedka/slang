#include "spplib.h"
#include <cstring>
#include <iostream>

// Конструктор по указателю и размеру
cpp_string::cpp_string(char *base, unsigned int size) : len(size) {
  text = (char *)malloc(len + 1);
  if (text) {
    memcpy(text, base, len);
    text[len] = '\0';
  }
}

// Конструктор по диапазону указателей
cpp_string::cpp_string(char *base, char *end) {
  len = end - base;
  text = (char *)malloc(len + 1);
  if (text) {
    memcpy(text, base, len);
    text[len] = '\0';
  }
}

// Конструктор по умолчанию
cpp_string::cpp_string() : len(0), text(nullptr) {}

// Копирующий конструктор
cpp_string::cpp_string(const cpp_string &other) : len(other.len) {
  if (other.text) {
    text = (char *)malloc(len + 1);
    memcpy(text, other.text, len + 1);
  } else {
    text = nullptr;
  }
}

// Перемещающий конструктор
cpp_string::cpp_string(cpp_string &&other) noexcept
    : len(other.len), text(other.text) {
  other.text = nullptr;
  other.len = 0;
}

// Копирующее присваивание
cpp_string &cpp_string::operator=(const cpp_string &other) {
  if (this != &other) {
    free(text);
    len = other.len;
    if (other.text) {
      text = (char *)malloc(len + 1);
      memcpy(text, other.text, len + 1);
    } else {
      text = nullptr;
    }
  }
  return *this;
}

// Перемещающее присваивание
cpp_string &cpp_string::operator=(cpp_string &&other) noexcept {
  if (this != &other) {
    free(text);
    text = other.text;
    len = other.len;
    other.text = nullptr;
    other.len = 0;
  }
  return *this;
}

// Добавление строки с realloc
void cpp_string::add(const cpp_string str) {
  if (!str.text)
    return; // если строка пустая, ничего не делаем

  char *new_text = (char *)realloc(text, len + str.len + 1);
  if (new_text) {
    text = new_text;
    memcpy(text + len, str.text, str.len);
    len += str.len;
    text[len] = '\0';
  }
}

// Простейший хэш (djb2)
unsigned int cpp_string::get_hash() {
  unsigned int hash = 5381;
  for (unsigned int i = 0; i < len; ++i) {
    hash = ((hash << 5) + hash) + text[i];
  }
  return hash;
}

// Печать строки
void cpp_string::print() {
  if (text)
    std::cout << text;
}

// Деструктор
cpp_string::~cpp_string() { free(text); }
