-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase macex
#pile

-- This file tests macro name scoping within add and with expressions.


macro importRecord(F) ==
        macro R(f) == Record(f,f)

        local
                Record:  (Type, Type) -> Type
                First:   Type
                Second:  Type
        local
                first:   First
                second:  Second
        import
                RecNew:  (F, F) -> R(F)
                RecElt:  (R(F), First)  -> F
                RecElt:  (R(F), Second) -> F
	from Builtin

macro  C == ComplexDoubleFloat

export C: with
        macro  F == DoubleFloat

        complex: (F,F) -> C
        real:    C -> F
        imag:    C -> F
        0:       C
        1:       C
        +:       (C, C) -> C
        -:       (C, C) -> C
        *:       (C, C) -> C
        /:       (C, C) -> C

C == add
        macro
            F == DoubleFloat
            R(f) == Record(f,f)

        export
                complex: (F,F) -> C
                real:    C -> F
                imag:    C -> F
                0:       C
                1:       C
                +:       (C, C) -> C
                -:       (C, C) -> C
                *:       (C, C) -> C
                /:       (C, C) -> C

        importRecord(DoubleFloat)

        complex(r: F, i: F): C == RecNew(r, i) pretend C
        real(a: C): F          == RecElt(a pretend R(F), first)
        imag(a: C): F          == RecElt(a pretend R(F), second)

        0: C == complex(0, 0)
        1: C == complex(1, 0)

        (a: C) + (b: C):C ==
                complex(real a + real b,  imag a + imag b)
        (a: C) - (b: C):C ==
                complex(real a - real b,  imag a - imag b)
        (a: C) * (b: C):C ==
                complex(real a * real b - imag a * imag b,
                        real a * imag b + imag a * real b)
        (a: C) / (b: C):C ==
                d: F == real b * real b  + imag b * imag b
                complex((real a *real b  + imag a * imag b)/d,
                        (imag a *real b  - real a * imag b)/d)

C      -- ComplexDoubleFloat
F      -- F
R(C)   -- R(ComplexDoubleFloat)
