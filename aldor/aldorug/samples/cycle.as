#include "aldor"
#include "aldorio"

import from Integer;

-- Multiple value returns and functional composition.
-- Only creating the closures by * should allocate storage.

I      ==> Integer;
III    ==> (I,I,I);
MapIII ==> (I,I,I) -> (I,I,I);

id: MapIII ==
        (i:I, j:I, k: I): III +-> (i,j,k);

(f: MapIII) * (g: MapIII): MapIII ==
        (i:I, j:I, k: I): III +-> f g (i,j,k);

(f: MapIII) ^ (p: Integer): MapIII == {
        p < 1  => id;
        p = 1  => f;
        odd? p => f*(f*f)^(p quo 2);
        (f*f)^(p quo 2);
}

-- test routine
main(): () == {
        cycle(a: I, b: I, c: I): III == (c, a, b);

        printIII(a: I, b: I, c: I): () == {
                stdout << "a = " << a << " b = "
                                        << b << " c = " << c << newline
        }
        printIII (cycle(1,2,3));
        printIII (cycle cycle  (1,2,3));
        printIII ((cycle*cycle)(1,2,3));
        printIII ((cycle^10)  (1,2,3));
}

main()

