#include <iostream>

#include "concise.h"

namespace {

template <tn T>
auto add(fix T& a, fix T& b) -> T {
  val c = a + b;
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
  expl Derived(fix i32 a_, fix i32 b_, fix i32 c_) : Class(a_, b_, c_) {}

  auto method() fix -> i32 override final { return 64; }

  auto pureVirtual() fix -> f32 override final { return 128f; }
};

auto main() -> int {
  var c = Derived{1, 2, 5};
  val a = c.a;
  val b = c.b;
  c.b = 3;
  val d = c.b;

  if (d == 2)
    return 1;
  else if (d == 3)
    return 0;
  else
    println("Continue");

  val tup = tuple{1i, 1.2l, "test"s};
  val uniqueValues = hashset<i32>{1, 2, 3};
  val basicMap = hashmap<str, i32>{{"k0", 0}, {"k1", 1}, {"k2", 2}};
  val listi = array{1, 2, 3};
  val listj = array{1, 2, 3};
  val listk = vector{1, 2, 3};
  val string = "This is a string"s;

  val s2 = 123s;

  val i1 = 123i;
  val i2 = 123l;

  val f1 = 123f;
  val f2 = 123.0f;
  val f3 = 123.0;
  val f4 = 123.0l;
  val position = 123z;

  println("Tuple: ", tup);
  println("Set: ", uniqueValues);
  println("Map: ", basicMap);
  println("Array: ", listi);
  println("Vector: ", listk);

  for (val[i, j, k] : zip(listi, listj, listk))
    println(i, ", ", j, ", ", k);

  for (val[pos, i, j, k] : ezip(listi, listj, listk))
    println("At position ", pos, ": ", i, ", ", j, ", ", k);

  for (val[pos, i] : enumerate(listi))
    println("Enumerate position ", pos, ": ", i);

  val d_string = std::invoke(
      [](val& a) {
        switch (a) {
          case 1:
            return "one";
          case 2:
            return "tow";
          default:
            return "unknown";
        }
      },
      d);
    
  println("d as string: ", d, " -> ", d_string);

  return 0;
}