#include "foamlib"
#pile

Z ==> MachineInteger

-- FiniteLinearAggregate T with
MyList(T: with): with
    nil: () -> %
    cons: (T, %) -> %
    empty?: % -> Boolean
    --bracket: Tuple T -> %
--    bracket: Generator T -> %
== add
    Rep == Record(first: T, rest: %)
    import from Rep, Pointer

    nil(): ? == (nil()$Pointer) pretend %
    cons(a: ?, r: ?): ? == per [a, r]
    empty?(l: ?): ? == nil?(l pretend Pointer)
#if 0
    [t: ?]: ? == generate
        head: % := last: % := nil()
        for a in g repeat
            temp := last
            last := cons(a, nil())
            empty? temp => head := last
            temp.rest := last
        head
#endif
