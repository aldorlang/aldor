#include "foamlib"

DomNameType ==>  'ID, APPLY, TUPLE, OTHER';

Rec: with {
     new: () -> %;
     tag: % -> DomNameType;
}
== add {
   Rep ==> Record(t: DomNameType);
   import from Rep;

   new(): % == per[ID];
   tag(x: %): DomNameType == rep(x).t;
}

