#include "aldor"

I ==> Integer;

HolderType(T: with): Category == with {
    content: % -> T;
}

GcdDomain: Category == with {
   gcd: % -> %;
}

FooCat: Category == with {
    f: % -> I;
    q: % -> I;
    val: I -> %;
    zz: (T: PrimitiveType) -> % -> T;

    default {
        gcd? ==> % has GcdDomain;
	f(n: %): I == {
	   gcd? => q(gcd n);
	   never;
	}
	zz(T: PrimitiveType)(x: %): T == if % has HolderType T then content x else never;
    }
}

FooDom: Join(GcdDomain, FooCat) with == add {
    Rep == I;
    import from I;
    q(x: %): I == rep x;
    val(n: I): % == per n;
    gcd(x: %): % == per(rep(x) + 100);
}

BarDom: Join(FooCat, HolderType I) with
 == add {
    Rep == I;
    import from I;
    q(x: %): I == rep x;
    val(n: I): % == per n;
    content(n: %): I == rep n;
}

test(): () == {
    import from Integer;
    v: FooDom := val 5;
    if f(v) ~= 105 then never;

    x: BarDom := val 2;
    if zz(I)(x) ~= 2 then never;
}

test();

