-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp

#include "axllib"

P(T: with): with {
   export from T;
}
== add;

BB: Category == with {
    a: () -> ()
};

Foo(S: with): with {
       if S has BB then foo: () -> ()
}
== add {
    import from P S;

    if S has BB then foo(): () == a()
}

Bar(S: with): with {
       if S has BB then foo: () -> ()
}
== add {
    import from P S;
    import from S;

    if S has BB then foo(): () == a()
}
