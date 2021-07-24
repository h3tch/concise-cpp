#pragma once
#pragma GCC diagnostic error "-Wall"
#pragma GCC diagnostic error "-Wextra"

#include "pipes/pipes.hpp"

#include <array>
#include <assert.h>
#include <exception>
#include <functional>
#include <inttypes.h>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define tn typename
#define fn auto
#define var auto
#define val const auto
#define fix const
#define me (*this)
#define virt virtual
#define stat static
#define cast static_cast
#define dcast dynamic_cast
#define rcast reinterpret_cast
#define expl explicit
#define impl implicit
#define noex noexcpet
#define eval_t decltype
#define decl_t decltype
#define base_t(T) std::remove_reference_t<std::remove_cv_t<decltype(T)>>
#define def constexpr
#define null nullptr
#define inl inline
#define enum enum class

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using iA = ssize_t;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using uA = size_t;
using f32 = float;
using f64 = double;
using f128 = long double;
using std::move;
using str = std::string;
using RuntimeError = std::runtime_error;
using InvalidArg = std::invalid_argument;
using OutOfRange = std::out_of_range;
using DomainError = std::domain_error;
using RangeError = std::range_error;
using LengthError = std::length_error;
using LogicError = std::logic_error;

#define tup std::tuple
#define arr std::array
#define vec std::vector
#define oset std::set;
#define omap std::map;
#define hmap std::unordered_map
#define hset std::unordered_set

#define sptr std::make_shared
#define uptr std::make_unique
#define wptr std::make_weak

// clang-format off
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wliteral-suffix"
constexpr i8  operator "" b (unsigned long long v) { return (i8)v; }
constexpr i16 operator "" s (unsigned long long v) { return (i16)v; }
constexpr i32 operator "" i (unsigned long long v) { return (i32)v; }
constexpr u8  operator "" ub(unsigned long long v) { return (u8)v; }
constexpr u16 operator "" us(unsigned long long v) { return (u16)v; }
constexpr u32 operator "" ui(unsigned long long v) { return (u32)v; }
constexpr uA  operator "" z (unsigned long long v) { return (uA)v; }
constexpr f32 operator "" f (unsigned long long v) { return (f32)v; }
std::string operator "" s (const char* s, std::size_t n) { return std::string{s, n}; }
static_assert(sizeof(1b) == 1);
static_assert(sizeof(1s) == 2);
static_assert(sizeof(1i) == 4);
static_assert(sizeof(1l) == 8);
static_assert(sizeof(1ub) == 1);
static_assert(sizeof(1us) == 2);
static_assert(sizeof(1ui) == 4);
static_assert(sizeof(1ul) == 8);
static_assert(sizeof(1.0) == 8);
static_assert(sizeof(1.0l) == 16);
#pragma GCC diagnostic pop
// clang-format on

template <typename Class, typename T>
class getter final {
  typedef T (*func)(Class&, const T&);
  typedef void (*delfunc)(Class&, T&);

 public:
  explicit getter(
      Class& owner,
      const T value,
      const func getter = [](Class&, const T& value) -> T { return value; },
      const delfunc deleter = [](Class&, T&) {})
      : v {value}
      , c {owner}
      , g {getter}
      , d {deleter} {}
  explicit getter(
      Class& owner,
      const T* value,
      const func getter = [](Class&, const T& value) -> T { return value; },
      const delfunc deleter = [](Class&, T& value) {})
      : v {*value}
      , c {owner}
      , g {getter}
      , d {deleter} {}
  ~getter() { d(c, v); }
  operator const T() const { return g(c, v); }

  using type = T;

 private:
  T v;
  Class& c;
  func g;
  delfunc d;
};

template <typename Class, typename T>
class getset final {
  typedef T (*func)(Class&, const T&);
  typedef void (*delfunc)(Class&, T&);

 public:
  explicit getset(
      Class& owner,
      const T value,
      const func setter = [](Class&, const T& value) -> T { return value; },
      const func getter = [](Class&, const T& value) -> T { return value; },
      const delfunc deleter = [](Class&, T&) {})
      : v {value}
      , c {owner}
      , g {getter}
      , s {setter}
      , d {deleter} {}
  explicit getset(
      Class& owner,
      const T* value,
      const func setter = [](Class&, const T& value) -> T { return value; },
      const func getter = [](Class&, const T& value) -> T { return value; },
      const delfunc deleter = [](Class&, T& value) {})
      : v {*value}
      , c {owner}
      , g {getter}
      , s {setter}
      , d {deleter} {}
  ~getset() { d(c, v); }
  auto operator=(const T& value) -> const T& { return v = s(c, value); }
  operator const T() const { return g(c, v); }

  using type = T;

 private:
  T v;
  Class& c;
  func g, s;
  delfunc d;
};

template <typename... T>
class zip_iterator {
 public:
  // member typedefs provided through inheriting from std::iterator
  class iterator
      : public std::iterator<std::input_iterator_tag,
                             std::tuple<typename T::iterator...>,
                             std::tuple<typename T::iterator...>,
                             const std::tuple<typename T::iterator...>*,
                             std::tuple<typename T::reference...>> {
    std::tuple<typename T::iterator...> iterators;

   public:
    explicit iterator(typename T::iterator... iterators)
        : iterators(std::make_tuple(iterators...)) {}

    iterator& operator++() {
      std::apply([](auto&... iterator) { std::make_tuple((++iterator)...); }, iterators);
      return *this;
    }

    iterator operator++(int) { return iterator(std::forward<T>(std::next(iterators))...); }

    bool operator==(const iterator& other) const { return iterators == other.iterators; }

    bool operator!=(const iterator& other) const { return iterators != other.iterators; }

    std::tuple<typename T::reference...> operator*() {
      return std::apply([](auto&... iterator) { return std::forward_as_tuple(*iterator...); },
                        iterators);
    }
  };

  zip_iterator(T&... container)
      : containers(std::forward_as_tuple(container...)) {
    static_assert(sizeof...(T) > 0, "At least one container is required for zipping.");
    const auto all_equal = [](auto const& c0, auto const&... c) {
      return ((c0.size() == c.size()) && ...);
    }(container...);
    if (not all_equal)
      throw std::invalid_argument("All containers need to have the same size for zipping.");
  }

  iterator begin() {
    return std::apply([](auto&... container) { return iterator(container.begin()...); },
                      containers);
  }

  iterator end() {
    return std::apply([](auto&... container) { return iterator(container.end()...); }, containers);
  }

 private:
  std::tuple<T&...> containers;
};

template <typename... T>
class const_zip_iterator {
 public:
  // member typedefs provided through inheriting from std::iterator
  class iterator
      : public std::iterator<std::input_iterator_tag,
                             std::tuple<typename T::const_iterator...>,
                             std::tuple<typename T::const_iterator...>,
                             const std::tuple<typename T::const_iterator...>*,
                             std::tuple<typename T::const_reference...>> {
    std::tuple<typename T::const_iterator...> iterators;

   public:
    explicit iterator(typename T::const_iterator... iterators)
        : iterators(std::make_tuple(iterators...)) {}

    iterator& operator++() {
      std::apply([](auto&... iterator) { std::make_tuple((++iterator)...); }, iterators);
      return *this;
    }

    iterator operator++(int) { return iterator(std::forward<T>(std::next(iterators))...); }

    bool operator==(const iterator& other) const { return iterators == other.iterators; }

    bool operator!=(const iterator& other) const { return iterators != other.iterators; }

    std::tuple<typename T::const_reference...> operator*() {
      return std::apply([](auto&... iterator) { return std::forward_as_tuple(*iterator...); },
                        iterators);
    }
  };

  const_zip_iterator(const T&... container)
      : containers(std::forward_as_tuple(container...)) {
    static_assert(sizeof...(T) > 0, "At least one container is required for zipping.");
    const auto all_equal = [](auto const& c0, auto const&... c) {
      return ((c0.size() == c.size()) && ...);
    }(container...);
    if (not all_equal)
      throw std::invalid_argument("All containers need to have the same size for zipping.");
  }

  iterator begin() {
    return std::apply([](auto&... container) { return iterator(container.cbegin()...); },
                      containers);
  }

  iterator end() {
    return std::apply([](auto&... container) { return iterator(container.cend()...); }, containers);
  }

 private:
  std::tuple<const T&...> containers;
};

template <typename... T>
auto zip(T&... container) -> zip_iterator<T...> {
  return zip_iterator<T...>(container...);
}

template <typename... T>
auto zip(const T&... container) -> const_zip_iterator<T...> {
  return const_zip_iterator<T...>(container...);
}

template <typename... T>
class indexed_zip_iterator {
 public:
  // member typedefs provided through inheriting from std::iterator
  class iterator
      : public std::iterator<std::input_iterator_tag,
                             std::tuple<size_t, typename T::iterator...>,
                             std::tuple<size_t, typename T::iterator...>,
                             const std::tuple<size_t, typename T::iterator...>*,
                             std::tuple<size_t, typename T::reference...>> {
    size_t position;
    std::tuple<typename T::iterator...> iterators;

   public:
    explicit iterator(size_t start_position, typename T::iterator... iterators)
        : position {start_position}
        , iterators(std::make_tuple(iterators...)) {}

    iterator& operator++() {
      position++;
      std::apply([](auto&... iterator) { std::make_tuple((++iterator)...); }, iterators);
      return *this;
    }

    iterator operator++(int) {
      return iterator(position + 1, std::forward<T>(std::next(iterators))...);
    }

    bool operator==(const iterator& other) const {
      return position == other.position and iterators == other.iterators;
    }

    bool operator!=(const iterator& other) const {
      return position != other.position or iterators != other.iterators;
    }

    std::tuple<size_t, typename T::reference...> operator*() {
      return std::apply(
          [this](auto&... iterator) { return std::forward_as_tuple(position, *iterator...); },
          iterators);
    }
  };

  indexed_zip_iterator(T&... container)
      : containers(std::forward_as_tuple(container...)) {
    static_assert(sizeof...(T) > 0, "At least one container is required for zipping.");
    const auto all_equal = [](auto const& c0, auto const&... c) {
      return ((c0.size() == c.size()) && ...);
    }(container...);
    if (not all_equal)
      throw std::invalid_argument("All containers need to have the same size for zipping.");
  }

  iterator begin() {
    return std::apply([](auto&... container) { return iterator(size_t {0}, container.begin()...); },
                      containers);
  }

  iterator end() {
    const auto end_position = std::get<0>(containers).size();
    return std::apply(
        [end_position](auto&... container) { return iterator(end_position, container.end()...); },
        containers);
  }

 private:
  std::tuple<T&...> containers;
};

template <typename... T>
class const_indexed_zip_iterator {
 public:
  // member typedefs provided through inheriting from std::iterator
  class iterator
      : public std::iterator<std::input_iterator_tag,
                             std::tuple<size_t, typename T::const_iterator...>,
                             std::tuple<size_t, typename T::const_iterator...>,
                             const std::tuple<size_t, typename T::const_iterator...>*,
                             std::tuple<size_t, typename T::const_reference...>> {
    size_t position;
    std::tuple<typename T::const_iterator...> iterators;

   public:
    explicit iterator(size_t start_position, typename T::const_iterator... iterators)
        : position {start_position}
        , iterators(std::make_tuple(iterators...)) {}

    iterator& operator++() {
      position++;
      std::apply([](auto&... iterator) { std::make_tuple((++iterator)...); }, iterators);
      return *this;
    }

    iterator operator++(int) {
      return iterator(position + 1, std::forward<T>(std::next(iterators))...);
    }

    bool operator==(const iterator& other) const {
      return position == other.position and iterators == other.iterators;
    }

    bool operator!=(const iterator& other) const {
      return position != other.position or iterators != other.iterators;
    }

    std::tuple<size_t, typename T::const_reference...> operator*() {
      return std::apply(
          [this](auto&... iterator) { return std::forward_as_tuple(position, *iterator...); },
          iterators);
    }
  };

  const_indexed_zip_iterator(const T&... container)
      : containers(std::forward_as_tuple(container...)) {
    static_assert(sizeof...(T) > 0, "At least one container is required for zipping.");
    const auto all_equal = [](auto const& c0, auto const&... c) {
      return ((c0.size() == c.size()) && ...);
    }(container...);
    if (not all_equal)
      throw std::invalid_argument("All containers need to have the same size for zipping.");
  }

  iterator begin() {
    return std::apply(
        [](auto&... container) { return iterator(size_t {0}, container.cbegin()...); }, containers);
  }

  iterator end() {
    const auto end_position = std::get<0>(containers).size();
    return std::apply(
        [end_position](auto&... container) { return iterator(end_position, container.cend()...); },
        containers);
  }

 private:
  std::tuple<const T&...> containers;
};

template <typename... T>
auto izip(T&... container) -> indexed_zip_iterator<T...> {
  return indexed_zip_iterator<T...>(container...);
}

template <typename... T>
auto izip(const T&... container) -> const_indexed_zip_iterator<T...> {
  return const_indexed_zip_iterator<T...>(container...);
}

template <typename T>
class indexed_iterator {
 public:
  // member typedefs provided through inheriting from std::iterator
  class iterator
      : public std::iterator<std::input_iterator_tag,
                             std::tuple<size_t, typename T::iterator>,
                             std::tuple<size_t, typename T::iterator>,
                             const std::tuple<size_t, typename T::iterator>*,
                             std::tuple<size_t, typename T::reference>> {
    typename T::iterator iter;
    size_t position;

   public:
    explicit iterator(typename T::iterator iter, size_t position)
        : iter(iter)
        , position(position) {}

    iterator& operator++() { return ++iter, ++position, *this; }

    iterator operator++(int) { return iterator(std::next(iter), position + 1); }

    bool operator==(const iterator& other) const { return iter == other.iter; }

    bool operator!=(const iterator& other) const { return iter != other.iter; }

    std::tuple<size_t, typename T::reference> operator*() { return {position, *iter}; }
  };

  indexed_iterator(T& container)
      : container(container) {}

  iterator begin() { return iterator(container.begin(), 0); }

  iterator end() { return iterator(container.end(), container.size()); }

 private:
  T& container;
};

template <typename T>
class const_indexed_iterator {
 public:
  // member typedefs provided through inheriting from std::iterator
  class iterator
      : public std::iterator<std::input_iterator_tag,
                             std::tuple<ssize_t, typename T::const_iterator>,
                             std::tuple<ssize_t, typename T::const_iterator>,
                             const std::tuple<ssize_t, typename T::const_iterator>*,
                             std::tuple<ssize_t, typename T::const_reference>> {
    typename T::const_iterator iter;
    size_t position;

   public:
    explicit iterator(typename T::const_iterator iter, size_t position)
        : iter(iter)
        , position(position) {}

    iterator& operator++() { return ++iter, ++position, *this; }

    iterator operator++(int) { return iterator(std::next(iter), position + 1); }

    bool operator==(const iterator& other) const { return iter == other.iter; }

    bool operator!=(const iterator& other) const { return iter != other.iter; }

    std::tuple<size_t, typename T::const_reference> operator*() { return {position, *iter}; }
  };

  const_indexed_iterator(const T& container)
      : container(container) {}

  iterator begin() { return iterator(container.cbegin(), 0); }

  iterator end() { return iterator(container.cend(), container.size()); }

 private:
  const T& container;
};

template <typename T>
auto indexed(T& container) -> indexed_iterator<T> {
  return indexed_iterator<T>(container);
}

template <typename T>
auto indexed(const T& container) -> const_indexed_iterator<T> {
  return const_indexed_iterator<T>(container);
}

template <class TupType, size_t... I>
void print_tuple(const TupType& _tup, std::index_sequence<I...>) {
  std::cout << "(";
  (..., (std::cout << (I == 0 ? "" : ", ") << std::get<I>(_tup)));
  std::cout << ")";
}

template <class T>
void print_container(const T& container) {
  std::cout << '[';
  for (auto i = container.begin(); i < container.end() - 1; ++i)
    std::cout << *i << ", ";
  std::cout << *(container.end() - 1) << ']';
}

template <class... T>
std::ostream& operator<<(std::ostream& os, const std::tuple<T...>& tuple) {
  print_tuple(tuple, std::make_index_sequence<sizeof...(T)>());
  return os;
}

template <class T, size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<T, N>& container) {
  std::cout << '[';
  for (auto i = container.begin(); i != container.end(); ++i)
    std::cout << (i == container.begin() ? "" : ", ") << *i;
  std::cout << ']';
  return os;
}

template <class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& container) {
  std::cout << '[';
  for (auto i = container.begin(); i != container.end(); ++i)
    std::cout << (i == container.begin() ? "" : ", ") << *i;
  std::cout << ']';
  return os;
}

template <class T>
std::ostream& operator<<(std::ostream& os, const std::set<T>& container) {
  std::cout << '{';
  for (auto i = container.begin(); i != container.end(); ++i)
    std::cout << (i == container.begin() ? "" : ", ") << *i;
  std::cout << '}';
  return os;
}

template <class K, class V>
std::ostream& operator<<(std::ostream& os, const std::map<K, V>& container) {
  std::cout << '{';
  for (auto i = container.begin(); i != container.end(); ++i)
    std::cout << (i == container.begin() ? "" : ", ") << i->first << ": " << i->second;
  std::cout << '}';
  return os;
}

template <class T>
std::ostream& operator<<(std::ostream& os, const std::unordered_set<T>& container) {
  std::cout << '{';
  for (auto i = container.begin(); i != container.end(); ++i)
    std::cout << (i == container.begin() ? "" : ", ") << *i;
  std::cout << '}';
  return os;
}

template <class K, class V>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<K, V>& container) {
  std::cout << '{';
  for (auto i = container.begin(); i != container.end(); ++i)
    std::cout << (i == container.begin() ? "" : ", ") << i->first << ": " << i->second;
  std::cout << '}';
  return os;
}

template <typename... T>
auto print() {}

template <typename printable_t, typename... T>
auto print(const printable_t& v, T... values) {
  std::cout << v;
  print(values...);
}

template <typename... T>
auto println(T... values) {
  print(values...);
  std::cout << '\n';
}

using namespace pipes;
