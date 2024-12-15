#include "foamlib"

LRec: with {
     new: () -> %;
}
== add {
   Elt ==> Record(t: MachineInteger);
   Rep ==> List Elt;
   import from Rep, Elt, MachineInteger;

   emptyRec(): Elt == [0];
   new(): % == per[emptyRec()];
}

