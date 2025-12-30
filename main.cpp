#include "ast/ast.h"
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Использование: mycompiler [опции] <файл1> <файл2> ...\n";
    return 1;
  }

  std::vector<std::string> inputFiles;
  std::string outputFile = "a.out"; // стандартное имя выходного файла
  bool verbose = false;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg == "-o") {
      if (i + 1 < argc) {
        outputFile = argv[++i];
      } else {
        std::cerr << "Ошибка: после -o нужно указать имя файла\n";
        return 1;
      }
    } else if (!arg.empty() && arg[0] == '-') {
      std::cerr << "Неизвестная опция: " << arg << "\n";
    } else {
      inputFiles.push_back(arg);
    }
  }

  if (inputFiles.empty()) {
    std::cerr << "Нет входных файлов для компиляции.\n";
    return 1;
  }

  // Здесь будет логика вашего компилятора
  for (const auto &f : inputFiles) {
    try {
      Parser parser(f);
      auto ast = parser.parse();
      ast->print();
    } catch (const std::exception &e) {
      std::cerr << "Parse error: " << e.what() << "\n";
      return 1;
    }
    return 0;
  }
  return 0;
}
