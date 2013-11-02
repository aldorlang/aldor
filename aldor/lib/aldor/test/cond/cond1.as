#include "aldor"

Q: Category == with;

Seg(X: with): with {
   if X has with { coerce: Integer -> % } then foo: Integer -> X;
}
== add {
   if X has with { coerce: Integer -> % } then foo(n: Integer): X == coerce(n)$X;
}

