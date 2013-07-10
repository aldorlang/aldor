
-- From bug 1261 by Saul Youssef

--> testint

#include "axllib"
#pile

MC ==> Record(Obj:Type,Mor:(Obj,Obj)->with)

define FunctorCategory(A:MC,B:MC):Category == with
    Obj:Type
    Mor:(Obj,Obj)->with
    
Functor(A:MC,B:MC):FunctorCategory(A,B) with == add
    (Obj:Type,Mor:(Obj,Obj)->with) == explode A

