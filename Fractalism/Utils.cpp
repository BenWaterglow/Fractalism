#include <fstream>

#include <Fractalism/Utils.hpp>

namespace fractalism::utils {

  std::string readFile(const char* filename) {
    std::ifstream file(filename, std::ios::binary);

    std::string result;
    file.seekg(0, std::ios::end);
    result.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(result.data(), result.length());
    return result;
  }

  void writeToFile(const char* filename, size_t length, const char* data) {
    std::ofstream file(filename, std::ios::binary);

    file.write(data, length);
  }
}