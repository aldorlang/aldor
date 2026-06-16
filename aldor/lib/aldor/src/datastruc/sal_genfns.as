#include "aldor"
#pile

Mapper(A: Type, B: Type): with
    mapper: (A -> B) -> %
    apply: (%, A) -> B
== add
    Rep ==> A -> B
    mapper(f: A -> B): % == per f
    apply(m: %, a: A): B ==
        f := rep m
        f a

Terminator(A: Type, B: Type): with
    terminator: (A -> Boolean, () -> B) -> %
    terminator: (A -> (), () -> B) -> %
    consume!: (%, A) -> Boolean
    result: % -> B
== add
    Rep == Cross(A -> Boolean, () -> B)

    terminator(c: A -> Boolean, r: () -> B): % ==
        t := (c, r)
	per t
	
    terminator(c: A -> (), r: () -> B): % ==
        terminator( (a: A): Boolean +-> {c a; false}, r)
	
    consume!(t: %, a: A): Boolean ==
        (c, r) := rep t
	c a

    result(t: %): B ==
        (c, r) := rep t
	r()

Terminator2(A: Type, B: Type): with
    terminator: (Generator A -> B) -> %
    run: (%, Generator A) -> B
== add
    Rep ==> Generator A -> B
    terminator(fn: Generator A -> B): % == per fn

    run(t: %, g: Generator A): B == rep(t)(g)

GeneratorFunctions(A: Type, B: Type): with
    (>>): (Generator A, Mapper(A, B)) -> Generator B
    (>>): (Generator A, Terminator(A, B)) -> B
    (>>): (Generator A, Terminator2(A, B)) -> B
    partitionBy: (g: Generator A, n: MachineInteger) -> Generator List A
== add
    import from List A
    
    (g: Generator A) >> (m: Mapper(A, B)): Generator B == generate
        for x in g repeat
	    yield m x

    (g: Generator A) >> (term: Terminator(A, B)): B == 
        for x in g repeat
	    done := consume!(term, x)
	    if done then return result term
	result term

    (g: Generator A) >> (term: Terminator2(A, B)): B == run(term, g)

    partitionBy(g: Generator A, n: MachineInteger): Generator List A == generate
        a: List A := []
	c := 0
        for x in g repeat
	    a := cons(x, a)
	    if c = n then
	        yield a
		(a, c) := ([], 0)
	if not empty? a then yield a

Consumers(A: OrderedArithmeticType): with
    min: () -> Terminator2(A, A)
    count: () -> Terminator(A, MachineInteger)
    export from GeneratorFunctions(A, A)
== add
    import from A
    min(): Terminator2(A, A) ==
        run(g: Generator A): A ==
	    cmin := 0
	    for x in g repeat
	        if x < cmin then cmin := x
	    cmin
	terminator run
    count(): Terminator(A, MachineInteger) ==
        c: MachineInteger := 0
	consume(n: A): () ==
	    free c
	    c := c + 1
	last(): MachineInteger == c
	terminator(consume, last)
	
#if ALDORTEST
#include "aldor"
#include "aldorio"
#pile

import from MachineInteger, Consumers MachineInteger
import from Assert MachineInteger
foo(): () ==
    r: List MachineInteger := [1,2,3]
    sz := (generator r) >> count()
    m := (generator r) >> min()
    assertEquals(m, 1)
    assertEquals(sz, 3)

bar(l: List MachineInteger): MachineInteger == generator(l) >> min()
#endif

