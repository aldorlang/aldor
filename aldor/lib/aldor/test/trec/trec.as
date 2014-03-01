#include "aldor"
#include "aldorio"

T: with == add;
R == Record(fn: T->T);
import from R;

foo(t0: T): T == {
    f := (t: T): T +-> t;
    r := [f];
    r.fn(t0)
}
