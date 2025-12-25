#include <iostream>
#include <string>
#include <vector>
#include "spp/spplib.h"

int main(int argc, char *argv[]) {
  // if (argc < 2) {
  //   std::cerr << "Использование: mycompiler [опции] <файл1> <файл2> ...\n";
  //   return 1;
  // }
  //
  // std::vector<std::string> inputFiles;
  // std::string outputFile = "a.out"; // стандартное имя выходного файла
  // bool verbose = false;
  //
  // for (int i = 1; i < argc; ++i) {
  //   std::string arg = argv[i];
  //
  //   if (arg == "-o") {
  //     if (i + 1 < argc) {
  //       outputFile = argv[++i];
  //     } else {
  //       std::cerr << "Ошибка: после -o нужно указать имя файла\n";
  //       return 1;
  //     }
  //   } else if (!arg.empty() && arg[0] == '-') {
  //     std::cerr << "Неизвестная опция: " << arg << "\n";
  //   } else {
  //     inputFiles.push_back(arg);
  //   }
  // }
  //
  // if (inputFiles.empty()) {
  //   std::cerr << "Нет входных файлов для компиляции.\n";
  //   return 1;
  // }

  // Здесь будет логика вашего компилятора
  // for (const auto &f : inputFiles) {
    auto reader = cpp_reader("test.spp");
    cpp_token *tok;
    do{
      tok = reader.get_token();
      tok->print();

    }while(tok->type!=CPP_EOF);
  // }
  //
  // std::cout << "Файл " << outputFile << " успешно сгенерирован.\n";
  //
  // return 0;
}
