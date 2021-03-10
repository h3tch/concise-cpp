#pragma once
#include <assert.h>
#include <inttypes.h>

#include <array>
#include <exception>
#include <functional>
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
#define var auto
#define val const auto
#define fix const
#define me (*this)
#define dyn virtual
#define sta static
#define cast static_cast
#define dcast dynamic_cast
#define rcast reinterpret_cast
#define expl explicit
#define impl implicit
#define noex noexcpet

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using f32 = float;
using f64 = double;
using f128 = long double;
using std::array;
using std::map;
using std::move;
using std::set;
using std::tuple;
using str = std::string;
using std::vector;
template <typename Key, typename T, typename Hash = std::hash<Key>,
          typename KeyEqual = std::equal_to<Key>,
          typename Allocator = std::allocator<std::pair<const Key, T>>>
using hashmap = std::unordered_map<Key, T, Hash, KeyEqual, Allocator>;
template <typename Key, typename Hash = std::hash<Key>,
          typename KeyEqual = std::equal_to<Key>,
          typename Allocator = std::allocator<Key>>
using hashset = std::unordered_set<Key, Hash, KeyEqual, Allocator>;
using RuntimeError = std::runtime_error;
using InvalidArg = std::invalid_argument;
using OutOfRange = std::out_of_range;
using DomainError = std::domain_error;
using RangeError = std::range_error;
using LengthError = std::length_error;
using LogicError = std::logic_error;

template <typename T>
using ptr = std::shared_ptr<T>;
template <typename T>
using uptr = std::unique_ptr<T>;
template <typename T>
using wptr = std::weak_ptr<T>;

// clang-format off
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wliteral-suffix"
int8_t      operator "" b (unsigned long long v) { return (int8_t)v; }
int16_t     operator "" s (unsigned long long v) { return (int16_t)v; }
int32_t     operator "" i (unsigned long long v) { return (int32_t)v; }
uint8_t     operator "" ub(unsigned long long v) { return (uint8_t)v; }
uint16_t    operator "" us(unsigned long long v) { return (uint16_t)v; }
uint32_t    operator "" ui(unsigned long long v) { return (uint32_t)v; }
size_t      operator "" z (unsigned long long v) { return (size_t)v; }
float       operator "" f (unsigned long long v) { return (float)v; }
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
  typedef const T (*func)(Class&, const T&);
  typedef void (*del)(Class&, T&);

 public:
  explicit getter(
      Class& owner, const T value,
      const func getter = [](Class&, const T& value) -> const T {
        return value;
      },
      const del deleter = [](Class&, T& value) {})
      : v{value}, c{owner}, g{getter}, d{deleter} {}
  explicit getter(
      Class& owner, const T* value,
      const func getter = [](Class&, const T& value) -> const T {
        return value;
      },
      const del deleter = [](Class&, T& value) {})
      : v{*value}, c{owner}, g{getter}, d{deleter} {}
  ~getter() { d(c, v); }
  operator const T() const { return g(c, v); }

 private:
  T v;
  Class& c;
  const func g;
  const del d;
};

template <typename Class, typename T>
class getset final {
  typedef const T (*func)(Class&, const T&);
  typedef void (*del)(Class&, T&);

 public:
  explicit getset(
      Class& owner, const T value,
      const func setter = [](Class&, const T& value) -> const T {
        return value;
      },
      const func getter = [](Class&, const T& value) -> const T {
        return value;
      },
      const del deleter = [](Class&, T& value) {})
      : v{value}, c{owner}, g{getter}, s{setter}, d{deleter} {}
  explicit getset(
      Class& owner, const T* value,
      const func setter = [](Class&, const T& value) -> const T {
        return value;
      },
      const func getter = [](Class&, const T& value) -> const T {
        return value;
      },
      const del deleter = [](Class&, T& value) {})
      : v{*value}, c{owner}, g{getter}, s{setter}, d{deleter} {}
  ~getset() { d(c, v); }
  auto operator=(const T& value) -> const T& { return v = s(c, value); }
  operator const T() const { return g(c, v); }

 private:
  T v;
  Class& c;
  const func g, s;
  const del d;
};

template <typename... T>
class zip_iterator {
 public:
  // member typedefs provided through inheriting from std::iterator
  class iterator : public std::iterator<
                       std::input_iterator_tag,  // iterator_category
                       std::tuple<typename T::iterator...>,  // value_type
                       std::tuple<typename T::iterator...>,  // difference_type
                       const std::tuple<typename T::iterator...>*,  // pointer
                       std::tuple<typename T::reference...>         // reference
                       > {
    std::tuple<typename T::iterator...> iterators;

   public:
    explicit iterator(typename T::iterator... iterators)
        : iterators(std::make_tuple(iterators...)) {}

    iterator& operator++() {
      std::apply([](auto&... iterator) { std::make_tuple((++iterator)...); },
                 iterators);
      return *this;
    }

    iterator operator++(int) {
      return iterator(std::forward<T>(std::next(iterators))...);
    }

    bool operator==(const iterator& other) const {
      return iterators == other.iterators;
    }

    bool operator!=(const iterator& other) const {
      return iterators != other.iterators;
    }

    std::tuple<typename T::reference...> operator*() {
      return std::apply(
          [](auto&... iterator) { return std::forward_as_tuple(*iterator...); },
          iterators);
    }
  };

  zip_iterator(T&... container)
      : containters(std::forward_as_tuple(container...)) {
    static_assert(sizeof...(T) > 0,
                  "At least one container is required for zipping.");
    const auto all_equal = [](auto const& c0, auto const&... c) {
      return ((c0.size() == c.size()) && ...);
    }(container...);
    if (not all_equal)
      throw std::invalid_argument(
          "All containers need to have the same size for zipping.");
  }

  iterator begin() {
    return std::apply(
        [](auto&... container) { return iterator(container.begin()...); },
        containters);
  }

  iterator end() {
    return std::apply(
        [](auto&... container) { return iterator(container.end()...); },
        containters);
  }

 private:
  std::tuple<T&...> containters;
};

template <typename... T>
class const_zip_iterator {
 public:
  // member typedefs provided through inheriting from std::iterator
  class iterator
      : public std::iterator<
            std::input_iterator_tag,                    // iterator_category
            std::tuple<typename T::const_iterator...>,  // value_type
            std::tuple<typename T::const_iterator...>,  // difference_type
            const std::tuple<typename T::const_iterator...>*,  // pointer
            std::tuple<typename T::const_reference...>         // reference
            > {
    std::tuple<typename T::const_iterator...> iterators;

   public:
    explicit iterator(typename T::const_iterator... iterators)
        : iterators(std::make_tuple(iterators...)) {}

    iterator& operator++() {
      std::apply([](auto&... iterator) { std::make_tuple((++iterator)...); },
                 iterators);
      return *this;
    }

    iterator operator++(int) {
      return iterator(std::forward<T>(std::next(iterators))...);
    }

    bool operator==(const iterator& other) const {
      return iterators == other.iterators;
    }

    bool operator!=(const iterator& other) const {
      return iterators != other.iterators;
    }

    std::tuple<typename T::const_reference...> operator*() {
      return std::apply(
          [](auto&... iterator) { return std::forward_as_tuple(*iterator...); },
          iterators);
    }
  };

  const_zip_iterator(const T&... container)
      : containters(std::forward_as_tuple(container...)) {
    static_assert(sizeof...(T) > 0,
                  "At least one container is required for zipping.");
    const auto all_equal = [](auto const& c0, auto const&... c) {
      return ((c0.size() == c.size()) && ...);
    }(container...);
    if (not all_equal)
      throw std::invalid_argument(
          "All containers need to have the same size for zipping.");
  }

  iterator begin() {
    return std::apply(
        [](auto&... container) { return iterator(container.cbegin()...); },
        containters);
  }

  iterator end() {
    return std::apply(
        [](auto&... container) { return iterator(container.cend()...); },
        containters);
  }

 private:
  std::tuple<const T&...> containters;
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
class enumerated_zip_iterator {
 public:
  // member typedefs provided through inheriting from std::iterator
  class iterator
      : public std::iterator<
            std::input_iterator_tag,                      // iterator_category
            std::tuple<size_t, typename T::iterator...>,  // value_type
            std::tuple<size_t, typename T::iterator...>,  // difference_type
            const std::tuple<size_t, typename T::iterator...>*,  // pointer
            std::tuple<size_t, typename T::reference...>         // reference
            > {
    size_t position;
    std::tuple<typename T::iterator...> iterators;

   public:
    explicit iterator(size_t start_position, typename T::iterator... iterators)
        : position{start_position}, iterators(std::make_tuple(iterators...)) {}

    iterator& operator++() {
      position++;
      std::apply([](auto&... iterator) { std::make_tuple((++iterator)...); },
                 iterators);
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
          [this](auto&... iterator) {
            return std::forward_as_tuple(position, *iterator...);
          },
          iterators);
    }
  };

  enumerated_zip_iterator(T&... container)
      : containters(std::forward_as_tuple(container...)) {
    static_assert(sizeof...(T) > 0,
                  "At least one container is required for zipping.");
    const auto all_equal = [](auto const& c0, auto const&... c) {
      return ((c0.size() == c.size()) && ...);
    }(container...);
    if (not all_equal)
      throw std::invalid_argument(
          "All containers need to have the same size for zipping.");
  }

  iterator begin() {
    return std::apply(
        [](auto&... container) {
          return iterator(size_t{0}, container.begin()...);
        },
        containters);
  }

  iterator end() {
    const auto end_position = std::get<0>(containters).size();
    return std::apply(
        [end_position](auto&... container) {
          return iterator(end_position, container.end()...);
        },
        containters);
  }

 private:
  std::tuple<T&...> containters;
};

template <typename... T>
class const_enumerated_zip_iterator {
 public:
  // member typedefs provided through inheriting from std::iterator
  class iterator
      : public std::iterator<
            std::input_iterator_tag,  // iterator_category
            std::tuple<size_t, typename T::const_iterator...>,  // value_type
            std::tuple<size_t,
                       typename T::const_iterator...>,  // difference_type
            const std::tuple<size_t,
                             typename T::const_iterator...>*,   // pointer
            std::tuple<size_t, typename T::const_reference...>  // reference
            > {
    size_t position;
    std::tuple<typename T::const_iterator...> iterators;

   public:
    explicit iterator(size_t start_position,
                      typename T::const_iterator... iterators)
        : position{start_position}, iterators(std::make_tuple(iterators...)) {}

    iterator& operator++() {
      position++;
      std::apply([](auto&... iterator) { std::make_tuple((++iterator)...); },
                 iterators);
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
          [this](auto&... iterator) {
            return std::forward_as_tuple(position, *iterator...);
          },
          iterators);
    }
  };

  const_enumerated_zip_iterator(const T&... container)
      : containters(std::forward_as_tuple(container...)) {
    static_assert(sizeof...(T) > 0,
                  "At least one container is required for zipping.");
    const auto all_equal = [](auto const& c0, auto const&... c) {
      return ((c0.size() == c.size()) && ...);
    }(container...);
    if (not all_equal)
      throw std::invalid_argument(
          "All containers need to have the same size for zipping.");
  }

  iterator begin() {
    return std::apply(
        [](auto&... container) {
          return iterator(size_t{0}, container.cbegin()...);
        },
        containters);
  }

  iterator end() {
    const auto end_position = std::get<0>(containters).size();
    return std::apply(
        [end_position](auto&... container) {
          return iterator(end_position, container.cend()...);
        },
        containters);
  }

 private:
  std::tuple<const T&...> containters;
};

template <typename... T>
auto ezip(T&... container) -> enumerated_zip_iterator<T...> {
  return enumerated_zip_iterator<T...>(container...);
}

template <typename... T>
auto ezip(const T&... container) -> const_enumerated_zip_iterator<T...> {
  return const_enumerated_zip_iterator<T...>(container...);
}

template <typename T>
class enumerate_iterator {
 public:
  // member typedefs provided through inheriting from std::iterator
  class iterator
      : public std::iterator<
            std::input_iterator_tag,                   // iterator_category
            std::tuple<size_t, typename T::iterator>,  // value_type
            std::tuple<size_t, typename T::iterator>,  // difference_type
            const std::tuple<size_t, typename T::iterator>*,  // pointer
            std::tuple<size_t, typename T::reference>         // reference
            > {
    typename T::iterator iter;
    size_t position;

   public:
    explicit iterator(typename T::iterator iter, size_t position)
        : iter(iter), position(position) {}

    iterator& operator++() { return ++iter, ++position, *this; }

    iterator operator++(int) { return iterator(std::next(iter), position + 1); }

    bool operator==(const iterator& other) const { return iter == other.iter; }

    bool operator!=(const iterator& other) const { return iter != other.iter; }

    std::tuple<size_t, typename T::reference> operator*() {
      return {position, *iter};
    }
  };

  enumerate_iterator(T& container) : container(container) {}

  iterator begin() { return iterator(container.begin(), 0); }

  iterator end() { return iterator(container.end(), container.size()); }

 private:
  T& container;
};

template <typename T>
class const_enumerate_iterator {
 public:
  // member typedefs provided through inheriting from std::iterator
  class iterator
      : public std::iterator<
            std::input_iterator_tag,  // iterator_category
            std::tuple<ssize_t, typename T::const_iterator>,  // value_type
            std::tuple<ssize_t, typename T::const_iterator>,  // difference_type
            const std::tuple<ssize_t, typename T::const_iterator>*,  // pointer
            std::tuple<ssize_t, typename T::const_reference>  // reference
            > {
    typename T::const_iterator iter;
    size_t position;

   public:
    explicit iterator(typename T::const_iterator iter, size_t position)
        : iter(iter), position(position) {}

    iterator& operator++() { return ++iter, ++position, *this; }

    iterator operator++(int) { return iterator(std::next(iter), position + 1); }

    bool operator==(const iterator& other) const { return iter == other.iter; }

    bool operator!=(const iterator& other) const { return iter != other.iter; }

    std::tuple<size_t, typename T::const_reference> operator*() {
      return {position, *iter};
    }
  };

  const_enumerate_iterator(const T& container) : container(container) {}

  iterator begin() { return iterator(container.cbegin(), 0); }

  iterator end() { return iterator(container.cend(), container.size()); }

 private:
  const T& container;
};

template <typename T>
auto enumerate(T& container) -> enumerate_iterator<T> {
  return enumerate_iterator<T>(container);
}

template <typename T>
auto enumerate(const T& container) -> const_enumerate_iterator<T> {
  return const_enumerate_iterator<T>(container);
}

template <class TupType, size_t... I>
void print_tuple(const TupType& _tup, std::index_sequence<I...>) {
  std::cout << "(";
  (..., (std::cout << (I == 0 ? "" : ", ") << std::get<I>(_tup)));
  std::cout << ")";
}

template <class T>
void print_container(const T& arr) {
  std::cout << '[';
  for (auto i = arr.begin(); i < arr.end() - 1; ++i) std::cout << *i << ", ";
  std::cout << *(arr.end() - 1) << ']';
}

template <class... T>
std::ostream& operator<<(std::ostream& os, const std::tuple<T...>& tup) {
  print_tuple(tup, std::make_index_sequence<sizeof...(T)>());
  return os;
}

template <class T, size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<T, N>& arr) {
  std::cout << '[';
  for (auto i = arr.begin(); i != arr.end(); ++i)
    std::cout << (i == arr.begin() ? "" : ", ") << *i;
  std::cout << ']';
  return os;
}

template <class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& arr) {
  std::cout << '[';
  for (auto i = arr.begin(); i != arr.end(); ++i)
    std::cout << (i == arr.begin() ? "" : ", ") << *i;
  std::cout << ']';
  return os;
}

template <class T>
std::ostream& operator<<(std::ostream& os, const std::set<T>& arr) {
  std::cout << '{';
  for (auto i = arr.begin(); i != arr.end(); ++i)
    std::cout << (i == arr.begin() ? "" : ", ") << *i;
  std::cout << '}';
  return os;
}

template <class K, class V>
std::ostream& operator<<(std::ostream& os, const std::map<K, V>& arr) {
  std::cout << '{';
  for (auto i = arr.begin(); i != arr.end(); ++i)
    std::cout << (i == arr.begin() ? "" : ", ") << i->first << ": "
              << i->second;
  std::cout << '}';
  return os;
}

template <class T>
std::ostream& operator<<(std::ostream& os, const std::unordered_set<T>& arr) {
  std::cout << '{';
  for (auto i = arr.begin(); i != arr.end(); ++i)
    std::cout << (i == arr.begin() ? "" : ", ") << *i;
  std::cout << '}';
  return os;
}

template <class K, class V>
std::ostream& operator<<(std::ostream& os,
                         const std::unordered_map<K, V>& arr) {
  std::cout << '{';
  for (auto i = arr.begin(); i != arr.end(); ++i)
    std::cout << (i == arr.begin() ? "" : ", ") << i->first << ": "
              << i->second;
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
