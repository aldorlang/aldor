#include "foamlib"
#pile

SetCategory: Category == with

Evalable(X: SetCategory): Category == with

Thing(R: Type): Category == with
    if R has SetCategory then
        if R has Evalable R then
	    foo: List % -> %
