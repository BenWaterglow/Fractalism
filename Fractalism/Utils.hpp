#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include <memory>
#include <algorithm>
#include <type_traits>

#include "Options.hpp"

#define UNUSED(...) ((void)__VA_ARGS__)

namespace utils {
  std::unique_ptr<char[]> format(const char *format, ...);
  std::unique_ptr<char[]> readFile(const char *filename);
  void writeToFile(const char *filename, size_t length, const char *data);

  template <typename E>
  static inline constexpr auto toUnderlyingType(E e) noexcept {
    return static_cast<std::underlying_type_t<E>>(e);
  }
  template <typename E>
  static inline constexpr E fromUnderlyingType(std::underlying_type_t<E> value) {
    return static_cast<E>(value);
  }

  template<options::Space space>
  constexpr bool enabled(const options::Space& toCheck) {
    return toUnderlyingType(space) & toUnderlyingType(toCheck);
  }

  template <class H, class T, auto callback>
  struct ChangeAware {
    ChangeAware(H& handler, T value) : handler(handler), value(value) {}
    template<typename...P>
    ChangeAware(H& handler, P...value) : handler(handler), value(value...) {}
    inline operator auto() const { return value; }
    inline T& operator=(const T& value) { this->value = value; callback(handler, this->value); return this->value; }
  private:
    H& handler;
    T value;
  };

  template <class T>
  struct Dirtyable {
    template<typename...P>
    Dirtyable(P...args) : value(args...), isDirty(true) {}
    Dirtyable(T value) : value(value), isDirty(true) {}
    operator T() const { return value; }
    T& operator=(const T& value) { isDirty = true;  return this->value = value; }
    T value;
    bool isDirty;
  };

  template<size_t charCount>
  struct String {
    const char value[charCount] = {};

    consteval String(const char(&str)[charCount]) {
      std::copy_n(str, charCount, (char *)value);
    }

    consteval bool operator==(const String<charCount> str) const {
      return std::equal(value, value + charCount, str.value);
    }

    template<size_t otherCharCount>
    consteval bool operator==(const String<otherCharCount> str) const {
      return false;
    }

    template<size_t otherCharCount>
    consteval String<charCount + otherCharCount - 1> operator+(const String<otherCharCount> str) const {
      char newChars[charCount + otherCharCount - 1] = {};
      std::copy_n(value, charCount - 1, newChars);
      std::copy_n(str.value, otherCharCount, newChars + charCount - 1);
      return newChars;
    }

    consteval char operator[](size_t i) const {
      return value[i];
    }

    consteval operator const char* () const {
      return value;
    }

    consteval size_t size() const {
      return charCount - 1;
    }
  };
}

template<size_t charCount, size_t otherCharCount>
static consteval utils::String<charCount + otherCharCount - 1> operator+(utils::String<charCount> lhs, const char (&rhs)[otherCharCount]) {
  return lhs + utils::String(rhs);
}

template<size_t charCount, size_t otherCharCount>
static consteval utils::String<charCount + otherCharCount - 1> operator+(const char(&lhs)[charCount], utils::String<otherCharCount> rhs) {
  return utils::String(lhs) + rhs;
}

template<utils::String str>
static consteval auto operator""_s() {
  return str;
}

#endif