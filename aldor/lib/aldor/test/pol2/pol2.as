#include "aldor"

Expr: Category == with;

C0: Category == Expr with {
    c: () -> ();
}

C1: Category == C0 with {
    default {
       c(): () == {}
    }
}
C2: Category == C0 with {
}

P0(X: Expr): Category == C0 with {
    if X has C1 then C1;
    if X has C2 then C2;
}

PD(R: Expr): P0(R) with {

} == add {

}

D1: C1 with == add {
}

D2: C2 with == add {
    c(): () == {}
}

foo(): () == {
   c()$PD(D1);
   c()$PD(D2);
}

foo()

