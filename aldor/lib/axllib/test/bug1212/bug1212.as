-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Original author: Ralf Hemmecke

--> testrun -Q1 -l axllib
--> testrun -Q3 -l axllib

#include "axllib"

define CAT: Category == DenseStorageCategory with {
        1: %;
    default {
        (p:TextWriter) << (x:%):TextWriter == p << "(SOMETHING)";
        (x: %) = (y: %): Boolean == true;
        one?(x: %): Boolean == x = 1;
    }
}

CalixPowerProducts(vars: List String): CAT with == {
    n: SingleInteger == #vars;
    add {
        Rep ==> PrimitiveArray SingleInteger;
        import from Rep;
        someFunction(T: PrimitiveArray %): () == {
                m: % := T.1;
        }
        1: % == per new(n, 0);
    }
}

main(): () == {
        import from List String;
        import from CalixPowerProducts(["x","y"]);
        e := 1;
        print << "(" << e << ")" << newline;
}
main();

