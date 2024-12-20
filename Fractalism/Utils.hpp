#ifndef _FRACTALISM_UTILS_HPP_
#define _FRACTALISM_UTILS_HPP_

#include <cmath>
#include <concepts>
#include <string>
#include <type_traits>

namespace fractalism {
  namespace utils {
    std::string readFile(const char* filename);
    void writeToFile(const char* filename, size_t length, const char* data);

    template<typename E>
    concept Enum = std::is_enum_v<E>;

    template <Enum E>
    static inline constexpr auto toUnderlyingType(E e) noexcept {
      return static_cast<std::underlying_type_t<E>>(e);
    }
    template <Enum E>
    static inline constexpr E fromUnderlyingType(std::underlying_type_t<E> value) {
      return static_cast<E>(value);
    }

    static inline constexpr std::wstring toSubscript(std::integral auto value) {
      // Silly, but we gotta handle 0 too. Might as well make a fast-path for single digits.
      size_t digitCount = value < 1 ? 1 : static_cast<size_t>(std::truncf(std::log10f(value))) + 1;
      std::wstring result;
      result.resize(digitCount);
      for (size_t i = 0; i < digitCount; i++) {
        result[i] = L'₀' + (value % 10);
        value /= 10;
      }
      return result;
    }

    template<Enum auto value>
    constexpr bool enabled(decltype(value) toCheck) {
      return toUnderlyingType(value) & toUnderlyingType(toCheck);
    }

    template<size_t stackBlockSize = 1024>
    static inline void memswap(auto* lhs, decltype(lhs) rhs, size_t size) {
      std::byte buffer[stackBlockSize]{};
      std::byte* a = reinterpret_cast<std::byte*>(lhs);
      std::byte* b = reinterpret_cast<std::byte*>(rhs);
      size_t position = 0;
      while (position < size) {
        size_t currentBlockSize = std::min(size - position, stackBlockSize);
        std::memcpy(buffer, a + position, currentBlockSize);
        std::memmove(a + position, b + position, currentBlockSize);
        std::memcpy(b + position, buffer, currentBlockSize);
        position += currentBlockSize;
      }
    }
  }
}
#endif