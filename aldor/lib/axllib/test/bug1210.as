
--> testerrs
-- Highly condensed version of bug 1210.

Category:with == add;
Type:Category == with;
Tuple(T:Type):with == add;
(args:Tuple Type) -> (results:Tuple Type):with == add;


define BasicType:Category == with;

Foo:BasicType with {foo: () -> %} == add {foo():% == Type pretend %;}
Bar:BasicType with {bar: () -> %} == add {bar():% == Type pretend %;}


curry(T:BasicType)(junk:Foo, t:T):() == {}


main():() ==
{
   import from Foo, Bar;

   -- This used to segfault the compiler. Now we just print a fairly
   -- ambiguous message indicating that type Bar is not suitable and
   -- that we expected one of type .... (and don't tell them which
   -- type that we expected!).
   show == curry(Foo);
   show(foo(), bar()); -- show: (Foo, Foo) -> ();
}


main();

