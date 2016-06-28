#include "comp"

#if ALDORTEST
#include "comp"
#include "aldorio"
#pile

testLetrec4(): () ==
    import from CTypeInfer, TypeEnv, Exp, TType, Subst
    prog: Exp := fromString "(lambda fn (letrec ((x (fn 2))) x))"
    (subst, tp) := ti(empty(), prog)
    stdout << "Subst " << subst << newline
    stdout << "tp " << subst tp << newline
testLetrec4()

testOrder(): () ==
    import from CTypeInfer, TType, Subst, String
    import from Symbol, List TType, List Constraint, Constraint
    tv1 := typeTVar(-"tv1")
    tv2 := typeTVar(-"tv2")
    tv3 := typeTVar(-"tv3")
    tv4 := typeTVar(-"tv4")
    tv5 := typeTVar(-"tv5")
    tv6 := typeTVar(-"tv6")
    C := [eq(tv3, tv1),iinst(tv4, tv2, [tv1]),eq(tv3, typeMap(int, tv2))]
    sigma := solve C
    stdout << "Subst: " << sigma << newline
    stdout << "TP: " << sigma typeMap(tv1, tv4) << newline


testOrder()

#endif
