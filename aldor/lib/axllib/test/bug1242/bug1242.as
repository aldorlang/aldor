-- Original by Saul Youssef

--> testint
--> testerrs

#include "axllib"
#pile

define Foo(A:Category):Category == with

define Foo2(A:Category):Category == Foo A with 

#if TestErrorsToo
define Foo3(A:Category):Category ==  with 
    F: (A,A) -> Foo A
    G: (A,A) -> Foo2 A
    default
        G(a1:A,a2:A):Foo2 A == F(a1,a2) add
#else
define Foo3(A:Category):Category ==  with 
    F: (A,A) -> Foo A with {}
    G: (A,A) -> Foo2 A with {}
    default
        G(a1:A,a2:A):Foo2 A == F(a1,a2) add
#endif

