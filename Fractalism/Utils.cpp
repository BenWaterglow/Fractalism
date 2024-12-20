#include <fstream>
#include <cstdarg>

#include <Fractalism/Utils.hpp>

namespace fractalism {

  std::unique_ptr<char[]> utils::readFile(const char* filename) {
    std::ifstream file(filename, std::ios::binary);

    file.seekg(0, std::ios::end);
    std::streampos length = file.tellg();
    file.seekg(0, std::ios::beg);

    if (length <= 0) {
      std::unique_ptr<char[]> str = std::make_unique<char[]>(1);
      str[0] = '\0';
      return str;
    }
    std::unique_ptr<char[]> str = std::make_unique<char[]>(((size_t)length) + 1);
    file.read(str.get(), length);
    str[length] = '\0';
    return str;
  }

  void utils::writeToFile(const char* filename, size_t length, const char* data) {
    std::ofstream file(filename, std::ios::binary);

    file.write(data, length);
  }
}