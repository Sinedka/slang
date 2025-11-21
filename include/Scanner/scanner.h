#include <fstream>
#include <string>

class SlFile {
private:

public:
  SlFile(std::string path) {
    std::ifstream in(path);
    if (!in) {
      throw std::invalid_argument("wrong path");
    }
    std::string text((std::istreambuf_iterator<char>(in)),
                     std::istreambuf_iterator<char>());
  }
};
