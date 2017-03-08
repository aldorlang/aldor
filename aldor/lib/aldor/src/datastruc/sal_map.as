#include "aldor.as"

MapperCategory(T: with, R: with): Category == with {
   apply: (%, T) -> R;
}

ListMapper(T: with, R: with): MapperCategory(List T, List R) with {
   map: (T -> R) -> %;
}
== add {
   Rep ==> (T -> R);
   map(f: T -> R): % == per f;
   apply(m: %, l: List T): List R == [(rep m)(x) for x in l];
}

#if ALDORTEST
#include "aldor"
#include "aldorio"

test(): () == {
    import from MachineInteger;
    import from List MachineInteger;
    import from ListMapper(MachineInteger, Integer);
    import from ListMapper(Integer, MachineInteger);
    import from Assert List MachineInteger;
    l: List MachineInteger := [1,2,3,4];
    l2 := map(coerce)(map(coerce)(l));
    assertEquals(l, l2);
}


#endif

