-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs

#include "axllib"
export A: Boolean;
export B: Boolean;

Foo: with {
     foo: () -> ();
}
== add {
     if B then { 
     	foo(): () == never;
     	foo(): () == never;
     }
     
}

