------------------------------- sal_intgmp.as -----------------------------
--
--  GMP integers
--
-- Copyright (c) Helene Prieto 2000-2020
-- Copyright (c) INRIA 2000, Version 9-2-2000
-- Logiciel Salli ©INRIA 2000, dans sa version du 9/2/2000
--
-- Updated: 2020 Peter Broadbery
---------------------------------------------------------------------------

#include "aldor"

#pile
macro
    Z == MachineInteger;
    PTR == Pointer;

#if ALDOC
\thistype{GMPInteger}
\History{Helene Prieto}{9/2/2000}{created}
\Usage{import from \this}
\Descr{\this~implements arbitrary precision integers with the integer
arithmetic provided by GMP.}
\begin{exports}
\category{\altype{CopyableType}}\\
\category{\altype{IntegerType}}\\
\alexp{coerce}: & \altype{AldorInteger} $\to$ \% & conversion\\
\alexp{coerce}: & \% $\to$ \altype{AldorInteger} & conversion\\
\alexp{limbs}:
& \% $\to$ \altype{Generator} \altype{MachineInteger} & iteration\\
\alexp{new}: & () $\to$ \% & initialization (for calling GMP)\\
\end{exports}
#endif

GMPInteger: Join(CopyableType, IntegerType) with
    coerce: AldorInteger -> %;
    coerce: % -> AldorInteger;
#if ALDOC
\alpage{coerce}
\Usage{m::\%\\ n::\altype{AldorInteger}}
\Signatures{
\name: & \altype{AldorInteger} $\to$ \%\\
\name: & \% $\to$ \altype{AldorInteger}\\
}
\Params{
{\em m} & \% & an \aldor integer\\
{\em n} & \altype{AldorInteger} & a gmp integer\\
}
\Descr{Those functions convert between \aldor and GMP integers.}
\Remarks{The conversion to an \altype{AldorInteger} can be
quadratic in the number of bits of the integer, which is expensive.}
#endif

	limbs: % -> Generator Z;
#if ALDOC
\alpage{limbs}
\Usage{ for d in \name~n repeat \{ \dots \} }
\Signature{\%}{\altype{Generator} \altype{MachineInteger}}
\Params{ {\em n} & \% & an integer\\ }
\Descr{This function allows the individual limbs of a GMP integer
to be iterated independently of the machine size.
This generator yields the limbs from the least to the most significant.}
\Remarks{The limbs of $n$ describe $|n|$, so combine it with
\alfunc{OrderedArithmeticType}{sign}
if you need a complete description of $n$.}
#endif
	new: () -> %;
#if ALDOC
\alpage{new}
\Signature{()}{\%}
\Retval{Returns an initialized GMP integer but with no value
stored into it. Results from this function can be used only
as parameters to explicit calls to {\tt mpz\_} functions.}
#endif

    export from IntegerSegment %;
== add
    import { int: Type; mpz__srcptr: Type; mpz__ptr: Type; mpf__ptr: Type; Ptr: Type } from Foreign C("gmp.h")
    import
       mpz__add: (mpz__ptr, mpz__srcptr, mpz__srcptr) -> ()
       mpz__and: (mpz__ptr, mpz__srcptr, mpz__srcptr) -> ()
       mpz__clear: mpz__ptr -> ()
       mpz__cmp: (mpz__srcptr, mpz__srcptr) -> int
       mpz__com: (mpz__ptr, mpz__srcptr) -> ()
       mpz__divexact: (mpz__ptr, mpz__srcptr, mpz__srcptr) -> ()
       mpz__fdiv__q__2exp: (mpz__ptr, mpz__srcptr, MachineInteger) -> ()
       mpz__gcd: (mpz__ptr, mpz__srcptr, mpz__srcptr) -> MachineInteger
       mpz__get__si: (mpz__srcptr) -> MachineInteger
       mpz__getlimbn: (mpz__ptr, int) -> MachineInteger
       mpz__init: mpz__ptr -> ()
       mpz__ior: (mpz__ptr, mpz__srcptr, mpz__srcptr) -> ()
       mpz__mul__2exp: (mpz__ptr, mpz__srcptr, MachineInteger) -> ()
       mpz__mul: (mpz__ptr, mpz__srcptr, mpz__srcptr) -> ()
       mpz__pow__ui: (mpz__ptr, mpz__srcptr, int) -> ()
       mpz__scan1: (mpz__srcptr, Z) -> Z
       mpz__set__si: (mpz__ptr, MachineInteger) -> ()
       mpz__set__str: (mpz__ptr, Ptr, int) -> ()
       mpz__sign: mpz__ptr -> ()
       mpz__sizeinbase: (mpz__srcptr, int) -> int
       mpz__size: (mpz__ptr) -> int
       mpz__set: (mpz__ptr, mpz__srcptr) -> int
       mpz__sub: (mpz__ptr, mpz__srcptr, mpz__srcptr) -> ()
       mpz__tdiv__q: (mpz__ptr, mpz__srcptr, mpz__srcptr) -> ()
       mpz__tdiv__qr: (mpz__ptr, mpz__ptr, mpz__srcptr, mpz__srcptr) -> ()
       mpz__tdiv__r: (mpz__ptr, mpz__srcptr, mpz__srcptr) -> ()
       mpz__tstbit: (mpz__ptr, MachineInteger) -> int
    from Foreign C("gmp.h")

    Rep == Pointer
    Rec64 ==> Record(szal:Z, lmbs:PTR);
    Rec32 ==> Record(szal:Z, sz: Z, lmbs:PTR);
    import from MachineInteger, Rep, Rec64
    local gmpIn(a: %): mpz__srcptr == rep(a) pretend mpz__srcptr
    local gmpIn(a: %): mpz__ptr == rep(a) pretend mpz__ptr
    local gmpOut(a: mpz__srcptr): % == per(a pretend Rep)

    (<<)(w: BinaryWriter, n: %): BinaryWriter == never
    (<<)(r: BinaryReader): % == never

    local wordsize:Z	== bytes;
    local b64?:Boolean	== wordsize=8;

    length(n: %): MachineInteger == mpz__sizeinbase(gmpIn n, (2@MachineInteger) pretend int) pretend MachineInteger

    free!(a: %): () == mpz__clear(gmpIn a)
    next(a:%):%	== a+1
    prev(a:%):%	== a-1
    zero?(a: %): Boolean == a = 0

    new(): % ==
        n: % := per([0, nil]$Rec64 pretend Pointer)
	mpz__init(gmpIn n)
	return n

    copy!(a:%, b:%):% ==
	zero? a or one? a => copy b
	mpz__set(gmpIn a, gmpIn b)
	return a

    copy(a:%):% ==
	zero? a or one? a => a;
	e:% := new();
	mpz__set(gmpIn  e, gmpIn a);
	return e

    integer(l: Literal): % ==
        e: % := new()
	mpz__set__str(gmpIn e, l pretend Ptr, (10@MachineInteger) pretend int)
	return e
	
    bit?(a: %, n: Z): Boolean  ==
        c: Z := mpz__tstbit(gmpIn a,n) pretend MachineInteger
	return c = 1;

    (a: %) + (b: %): % ==
	e: % := new()
	mpz__add(gmpIn e, gmpIn a, gmpIn b)
	return e

    (*)(a: %, b: %): % ==
	e: % := new()
	mpz__mul(gmpIn e, gmpIn a, gmpIn b)
	return e

    (a: %) = (b: %): Boolean ==
	r := mpz__cmp(gmpIn a, gmpIn b)
	(r pretend MachineInteger) = 0

    (<)(a: %, b: %): Boolean ==
	r := mpz__cmp(gmpIn a, gmpIn b)
	(r pretend MachineInteger) < 0

    (<=)(a: %, b: %): Boolean ==
	r := mpz__cmp(gmpIn a, gmpIn b)
	(r pretend MachineInteger) <= 0

    (rem)(a: %, b: %): % ==
        e: % := new()
        mpz__tdiv__r(gmpIn e, gmpIn a, gmpIn b);
	return e
	
    (quo)(a: %, b: %): % ==
        e: % := new()
        mpz__tdiv__q(gmpIn e, gmpIn a, gmpIn b);
	return e

    (^)(n: %, d: Z): % ==
        e: % := new()
        mpz__pow__ui(gmpIn e, gmpIn n, d pretend int);
	return e

    machine(n: %): MachineInteger  == mpz__get__si(gmpIn n)

    0: % == coerce 0

    1: % == coerce 1

    coerce(n: MachineInteger): % ==
       h: % := new()
       mpz__set__si(gmpIn h, n)
       return h

    -(a: %): % ==
       zero? a => a
       b: % := new()
       mpz__sub(gmpIn b, gmpIn 0, gmpIn a)
       return b

    gcd(a: %, b: %): % ==
        e: % == new()
	mpz__gcd(gmpIn e, gmpIn a, gmpIn b)
	return e

    lcm(a: %, b: %): % ==
	ab := a * b;
	c := gcd(a, b);
	mpz__divexact(gmpIn ab, gmpIn ab, gmpIn c);
	ab;

    (\/)(a: %, b: %): % ==
        e: % == new()
	mpz__ior(gmpIn e, gmpIn a, gmpIn b)
	return e

    (/\)(a: %, b: %): % ==
        e: % == new()
	mpz__and(gmpIn e, gmpIn a, gmpIn b)
	return e

    (~)(n: %): % ==
        e: % := new()
	mpz__com(gmpIn e, gmpIn n)
	return e

    divide(p: %, q: %): (%, %)  ==
	e:% := new()
	d:% := new()
	mpz__tdiv__qr(gmpIn e, gmpIn d, gmpIn p, gmpIn q)
	(e,d);

    shift!(a:%,b:Z):%	==
	zero? a or one? a => shift(a, b)
	if b > 0 then mpz__mul__2exp(gmpIn a, gmpIn a, b)
		else mpz__fdiv__q__2exp(gmpIn a, gmpIn a, -b);
	a

    shift(a:%,b:Z):%	==
	c:% := new()
	if b > 0 then mpz__mul__2exp(gmpIn c, gmpIn a, b)
		else mpz__fdiv__q__2exp(gmpIn c, gmpIn a, -b)
	c

    sign(a:%):Z ==
        r := mpz__cmp(gmpIn a, gmpIn 0)
	return r pretend MachineInteger

    nthRoot(x:%, e:%):(Boolean,%)	==
	import from IntegerTypeTools %;
	binaryNthRoot(x,e);

    (p:TextWriter) << (x:%):TextWriter ==
	import from IntegerTypeTools %;
        print(x, 10@%, p);

    << (p:TextReader):% ==
	import from IntegerTypeTools %;
	scan p;

    random(): % ==
        import from RandomNumberGenerator
        return randomInteger()::%

    random(n: MachineInteger): % ==
	import from RandomNumberGenerator;
	assert(n > 0);
	r:% := 0;
	m := 8 * bytes$Z;
	for i in 1..n repeat
		r := add!(shift!(r, m), randomInteger()::%);
	return r;

    local nlimbs(n: %): Z == mpz__size(gmpIn n) pretend Z

    limbs(n: %): Generator Z == generate
        sz: Z := nlimbs n
	for i in 0..sz-1 repeat
	    yield mpz__getlimbn(gmpIn n, i pretend int) pretend Z

    -- these may work, but need testing
    coerce(x:%): AldorInteger ==
	zero?(sgn := sign x) => 0;
        (s, r) := divide(length x, 8);
        if ~zero? r then s := next s;
	a:AldorInteger := 0;
	st:Z := 0;
	for n in limbs x repeat
	    nn := n;
	    for m in 1..wordsize repeat
		a := a \/ shift((nn /\ 255)::AldorInteger, st);
		nn := shift(nn, -8);
		st := st + 8;
	sgn < 0 => -a;
	a;

    coerce(a:AldorInteger):% ==
        coerce1(n: AldorInteger): % ==
	    zero? n => 0
	    (q, r) := divide(n, 256)
	    coerce1(q) * 256 + (machine(r)::%)
	zero? a => 0
	a < 0 => -coerce1(-a)
	coerce1(a)

#if 0
	macro PZ == Record(z:Z);
        import from Z, Boolean, PZ;
	zero?(sgn := sign a) => 0;
	if sgn < 0 then a := -a;
        (s, r) := divide(length a, 8);
        if ~zero? r then s := next s;
	x: % := new();
	mpz__set__si(gmpIn x, 0);			-- x = 0
	mpz__setbit(gmpIn x, b := 8 * s);		-- x = 2^(8s)
	ptr := plimbs x;
	assert(~nil? ptr);
	while s > 0 repeat 			-- must scan s bytes
	    n:Z := 0;
	    st:Z := 0;
	    for i in 1..wordsize while s > 0 repeat
	        n := n \/ shift(machine(a) /\ 255, st);
		st := st + 8;
		s := prev s;
		a := shift(a, -8);
	    (ptr pretend PZ).z := n;
	    ptr := (ptr::Z + wordsize)::PTR;
	mpz__clrbit(rep x, b);
	if sgn < 0 then mpz__neg(rep x, rep x);
	x;
#endif

#if ALDORTEST
---------------------- test sal_intgmp.as --------------------------
#include "aldor"
#include "aldortest"
#pile

import from Assert GMPInteger
import from Assert String
import from Assert Boolean
import from GMPInteger
Z ==> GMPInteger

local test():Boolean ==
	assertEquals("12345", toString 12345)
	true

local testZero(): Boolean ==
    assertEquals(0@Z, (-1) + 1@Z)
    assertTrue(zero? 0)
    true

local testArith(): Boolean ==
    import from MachineInteger
    for i in 1..10@MachineInteger repeat
        r0: Z := random(i)
        r1: Z := random(i)
	g := gcd(r0, r1)
	assertEquals(0, r0 rem g)
	assertEquals(0, r1 rem g)
	assertEquals(0, r0-r0)
	assertEquals(0, r1-r1)
	assertEquals(2, (r0+r0) quo r0)
	assertEquals(r1, (r0*r1) quo r0)
    true

local testBit(): Boolean ==
    import from Fold Z, MachineInteger, Boolean
    SZ: MachineInteger := 30
    acc(n: Z, d: Z): Z == n * 2 + d
    l: List Z := [random(1$MachineInteger) rem 2 for i in 1@MachineInteger..SZ]
    n := (acc)/reverse l
    for i in 0..SZ-1 repeat
	assertTrue( (if bit?(n, i) then 1 else 0) = l.(i+1))
    true

stderr << "Testing sal__gmp2..." << newline
aldorTest("test", test)
aldorTest("testZero", testZero)
aldorTest("testArith", testArith)
aldorTest("testBit", testBit)
stderr << newline
#endif
