#include "aldor"
#pile

TypedPointer(T: with): HashType with
    if T has OutputType then OutputType
    pointer: T -> %
    value: % -> T
== add
    Rep ==> Pointer
    import from Rep
    import from T
    (a: %) = (b: %): Boolean == rep(a) = rep(b)
    hash(a: %): MachineInteger == hash rep a
    pointer(t: T): % == per(t pretend Pointer)
    value(p: %): T == rep(p) pretend T

    if T has OutputType then
            (o: TextWriter) << (a: %): TextWriter == o << value a

#if ALDORTEST
#include "comp"
#pile

test(): () ==
    import from TypedPointer String
    import from String
    import from Assert TypedPointer String
    import from Assert String
    s := "hello"
    s2 := copy s
    assertEquals(s, s2)
    assertNotEquals(pointer s, pointer s2)
    assertEquals(value pointer s, value pointer s2)

test()
#endif
