#include "concise.h"

namespace {

template <tn T>
auto inl add(fix T& a, fix T& b) -> T {
  val c = a + b;
  return c;
}

}  // namespace

auto cexpr addi(fix i32 a, fix i32 b) -> i32 { return a + b; }

class Class {
 private:
  i32 c;

 public:
  getter<Class, i32> a;
  getset<Class, i32> b;

  Class(fix i32 a_, fix i32 b_, fix i32 c_)
      : a(me, a_,
          [](Class& self, fix i32& v) -> fix i32 { return v + self.c; }),
        b(me, b_,
          [](Class& self, fix i32& v) -> fix i32 { return v - self.c; }),
        c(c_) {}

  auto sta static_method() -> i32 { return cast<i32>(32f); }

  auto dyn method() fix -> i32 { return c; }

  auto dyn pureVirtual() fix -> f32 = 0;
};

class Derived : public Class {
 public:
  exp Derived(fix i32 a_, fix i32 b_, fix i32 c_) : Class(a_, b_, c_) {}

  auto method() fix -> i32 final { return 64; }

  auto pureVirtual() fix -> f32 final { return 128f; }
};

auto main() -> int {
  var c = Derived{1, 2, 5};
  val a = c.a;
  val b = c.b;
  c.b = 3;
  val d = c.b;
  decl(d)::type e = 6;
  var& c2 = dcast<Class&>(c);

  val p = uptr<int>(123);
  val q = sptr<int>(234);
  val [m, n] = tup{1, 2};
  val [s, t] = tup{arr{1, 2}, vec{3, 4}};

  if (d == 2)
    return 1;
  else if (d == 3)
    return 0;
  else
    println("Continue");

  val tuple = tup{1i, 1.2l, "test"s};
  val uniqueValues = hset{1, 2, 3};
  val basicMap = hmap<str, i32>{{"k0", 0}, {"k1", 1}, {"k2", 2}};
  val listi = arr{1, 2, 3};
  val listj = arr{11, 22, 33};
  val listk = vec{111, 222, 333};
  val string = "This is a string"s;

  val s2 = 123s;

  val i1 = 123i;
  val i2 = 123l;

  val f1 = 123f;
  val f2 = 123.0f;
  val f3 = 354'123.0;
  val f4 = 123.0l;
  val position = 123z;

  println("Tuple: ", tuple);
  println("Set: ", uniqueValues);
  println("Map: ", basicMap);
  println("Array: ", listi);
  println("Vector: ", listk);
  println("Nested: ", tup{arr{1, 2, 3}, vec{4, 5, 6}});

  for (val[i, j, k] : zip(listi, listj, listk)) println(i, ", ", j, ", ", k);

  for (val[pos, i, j, k] : ezip(listi, listj, listk))
    println("At position ", pos, ": ", i, ", ", j, ", ", k);

  return 0;
}