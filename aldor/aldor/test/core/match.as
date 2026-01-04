#include "foamlib"
#pile
#library M "m0.ao"
import from M
#library ML "matchlist.ao"
import from ML

import from MachineInteger
import from LList MachineInteger

Foo(T: BasicType): with
    eq: T -> T -> PPartial T
    export from T
== add
    eq(x: T)(y: T): PPartial(T) == never

test1(): Boolean ==
    l1 := cons(1, nil())
    l1 case isNil()


test2(): Boolean ==
    l1 := cons(1, nil())
    l1 case cons(?,?)
    false


test3(): Boolean ==
    import from Foo MachineInteger
    12 case eq(1)(?)

test4(): Boolean ==
    12 case (?)

#if 0
--later
test5(): Boolean == select g in
    a*x^n + ? => ...
#endif    

#if 0
test5(): () ==
    foo(l: LList MachineInteger): MachineInteger ==
        select l in
	    nil() => 1
	    cons(?, nil) => 2
    	    ? => 3
	foo(nil())

foo(): () ==
    select (a, b) in
        (cons(?ta, b), ?) and (...) => ...

add(a, b) ==
    select (a, b) in
        (?, 0) => a
        (0, ?) => b
	(split(?p1, ?n, ?r1), split(?p2, ?n, r1)) => (p1+p2)*x^n + add(r1, r2)
	(split(?p1, ?n1, ?r1), split(?p2, ?n2, r1)) =>
	    n1 > n2 => p1*x^n1 + add(r1, b)
	    p2*x^n2 + add(a, r1)
	
#endif



--test6(): Boolean ==
--    nil() case [?, ?] => true
--    false

test1()
test2()
test3()
test4()
