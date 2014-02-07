#include "aldor"

Z: with == add;

XXX: Category == with;

I: XXX with { c: % -> Z } == add { c(a: %): Z == never }

GC(T: with): Category == with { generator: % -> Generator T}

Seg(A: with): with {
  if A has XXX and A has with { c: % -> Z } then GC(A);
  new: () -> %
} == add {
   if A has with { c: % -> Z } then {
       generator(a: %): Generator A == never;
   }
   new(): % == never;
}

foo(): () == {
  import from Seg I;
  s: Seg I := new();
  generator s
}
