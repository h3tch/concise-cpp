#include <iostream>

#include "concise.h"

template <tn T>
auto add(fix T a, fix T b) -> T
{
  var c = a + b;
  return c;
}

auto addi(fix i32 a, fix i32 b) -> i32
{
    return a + b;
}

class Class {
public:
    Class(fix i32 a_, fix i32 b_, fix i32 c_)
        : a(me, a_, [](Class& self, fix i32& v) -> fix i32 { return v + self.c; })
        , b(me, b_, [](Class& self, fix i32& v) -> fix i32 { return v - self.c; })
        , c(c_)
    {}
    getter<Class, i32> a;
    getset<Class, i32> b;

    virtual auto function(fix i32 a) -> bool { return a % 2 == 0; }

private:
    i32 c;
};

class Class2 : public Class {
public:
    auto function(fix i32 a) -> bool final override { return a % 2 == 1; }
};

auto main() -> int {
  mut c = Class{1, 2, 5};
  var a = c.a;
  var b = c.b;
  c.b = 3;
  var d = c.b;

  if (d == 2)
    return 1;
  else if (d == 3)
    return 0;
  else
    println("Continue");

  var tup = tuple{1i, 1.2l, "test"s};
  var unique_values = hashset<i32>{1, 2, 3};
  var basic_map = hashmap<str, i32>{{"k0", 0}, {"k1", 1}, {"k2", 2}};
  var listi = array{1, 2, 3};
  var listj = array{1, 2, 3};
  var listk = vector{1, 2, 3};
  var string = "This is a string"s;

  var s2 = 123s;

  var i1 = 123i;
  var i2 = 123l;

  var f1 = 123f;
  var f2 = 123.0f;
  var f3 = 123.0;
  var f4 = 123.0l;
  var position = 123z;

  println("Tuple: ", tup);
  println("Set: ", unique_values);
  println("Map: ", basic_map);
  println("Array: ", listi);
  println("Vector: ", listk);

  for (var[i, j, k] : zip(listi, listj, listk)) {
    println(i, ", ", j, ", ", k);
  }

  for (var[pos, i, j, k] : ezip(listi, listj, listk)) {
    println("At position ", pos, ": ", i, ", ", j, ", ", k);
  }

  return 0;
}