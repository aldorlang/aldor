-----------------------------------------------------------------
----
---- imod.as: Modular integer arithmetic.
----
-----------------------------------------------------------------

#include "aldor"

define ModularIntegerType(I: IntegerType):Category ==
  ArithmeticType  with {
    integer:Literal -> %;
    coerce: I -> %;
    lift:   % -> I;
    inv:    % -> %;
    /:      (%, %) -> %;
}


ModularIntegerNumberRep(I: IntegerType)(n: I):
    ModularIntegerType(I) with
== add {
    Rep  == I;
    import from Rep;

    0: % == per 0;
    1: % == per 1;

    (^)(x:%,n:MachineInteger):% ==
        binaryExponentiation!(x, n)$BinaryPowering(%,MachineInteger);

    (x: %) * (y: %): % == per((rep x * rep y) mod n);
    commutative?: Boolean == true;

    (port: TextWriter) << (x: %): TextWriter == port << rep x;

    coerce (i: I): %          == per(i mod n);
    integer(l: Literal): %    == per(integer l mod n);
    lift   (x: %): I          == rep x;

    zero?(x: %): Boolean      == x = 0;
    (x: %) = (y: %): Boolean  == rep(x) = rep(y);
    (x: %)~= (y: %): Boolean  == rep(x) ~= rep(y);

    - (x: %): %        == if x = 0 then 0 else per(n - rep x);
    (x: %) + (y: %): % ==
        per(if (z := rep x-n+rep y) < 0 then z+n else z);
    (x: %) - (y: %): % ==
        per(if (z := rep x  -rep y) < 0 then z+n else z);

    (x: %) / (y: %): % == x * inv y;

    inv(j: %): % == {
        local c0, d0, c1, d1: Rep;
        (c0, d0) := (rep j, n);
        (c1, d1) := (rep 1, 0);
        while not zero? d0 repeat {
            q := c0 quo d0;
            (c0, d0) := (d0, c0 - q*d0);
            (c1, d1) := (d1, c1 - q*d1)
        }
        assert(c0 = 1);
        if c1 < 0  then c1 := c1 + n;
        per c1
    }
}


SI ==> MachineInteger;
Z ==> Integer;

MachineIntegerMod(n: SI): ModularIntegerType(SI) with {
    lift: % -> Z;
} == ModularIntegerNumberRep(SI)(n) add {
    Rep == SI;

    lift(x: %): Z == (rep x)::Z;

    (x: %) * (y: %): % == {
        import from Machine;
        (xx, yy) := (rep x, rep y);
        xx = 1 => y;
        yy = 1 => x;
        -- Small case
        HalfWord ==> 32767; --!! Should be based on max$Rep
        (n < HalfWord) or (xx<HalfWord and yy<HalfWord) => (xx*yy)::%;

        -- Large case
        (nh, nl) := double_*(xx pretend Word, yy pretend Word);
        (qh, ql, rm) := doubleDivide(nh, nl, n pretend Word);
        rm pretend %;
    }
}


IntegerMod(n: Z): ModularIntegerType(Z) with {
    coerce: SI -> %;
} == ModularIntegerNumberRep(Z)(n) add {

    coerce(i:SI):% == (i::Z)::%;
}

