#include <iostream>

#include "concise.h"

namespace {

template <tn T>
auto add(fix T& a, fix T& b) -> T {
  var c = a + b;
  return c;
}

}  // namespace

auto addi(fix i32 a, fix i32 b) -> i32 { return a + b; }

class Class {
 public:
  getter<Class, i32> a;
  getset<Class, i32> b;

  Class(fix i32 a_, fix i32 b_, fix i32 c_)
      : a(me, a_,
          [](Class& self, fix i32& v) -> fix i32 { return v + self.c; }),
        b(me, b_,
          [](Class& self, fix i32& v) -> fix i32 { return v - self.c; }),
        c(c_) {}

  sta auto static_method() -> i32 { return cast<i32>(32f); }

  dyn auto method() fix -> i32 { return c; }

  dyn auto pureVirtual() fix -> f32 = 0;

 private:
  i32 c;
};

class Derived : public Class {
 public:
  exp Derived(fix i32 a_, fix i32 b_, fix i32 c_) : Class(a_, b_, c_) {}

  auto method() fix -> i32 final { return 64; }

  auto pureVirtual() fix -> f32 final { return 128f; }
};

auto main() -> int {
  mut c = Derived{1, 2, 5};
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
  var uniqueValues = hashset<i32>{1, 2, 3};
  var basicMap = hashmap<str, i32>{{"k0", 0}, {"k1", 1}, {"k2", 2}};
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
  println("Set: ", uniqueValues);
  println("Map: ", basicMap);
  println("Array: ", listi);
  println("Vector: ", listk);

  for (var[i, j, k] : zip(listi, listj, listk))
    println(i, ", ", j, ", ", k);

  for (var[pos, i, j, k] : ezip(listi, listj, listk))
    println("At position ", pos, ": ", i, ", ", j, ", ", k);

  return 0;
}