#ifndef _FRACTALISM_PROXY_HPP_
#define _FRACTALISM_PROXY_HPP_

#include <type_traits>

namespace fractalism {
  namespace proxy {

    template<class P, typename T>
    concept Proxy = requires(P & p, T & t) {
      { p.unwrap() } -> std::convertible_to<std::remove_cvref_t<T>>;
      { p.unwrapValue() } -> std::convertible_to<std::remove_cvref_t<T>>;
      p = t;
      p.unwrap() = p;
      t = p.unwrap();
    };

    template <typename T, auto callback>
    struct CallbackProxy {
      CallbackProxy(T value) : value(value) {}
      template<typename...P>
      CallbackProxy(P...value) : value(value...) {}
      template<typename R = T> requires std::convertible_to<std::remove_cvref_t<T>&, std::remove_cvref_t<R>&>
      inline R& unwrap() const { return value; }
      template<typename R = T> requires std::convertible_to<std::remove_cvref_t<T>, std::remove_cvref_t<R>>
      inline R unwrapValue() const { return value; }
      inline operator auto& () const { return value; }
      inline bool operator==(const auto& value) {
        return this->value == value;
      }
      inline T& operator=(const auto& value) {
        this->value = value;
        callback();
        return this->value;
      }
    private:
      mutable T value;
    };
  }
}

#endif