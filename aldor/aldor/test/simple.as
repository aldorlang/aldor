#include "foamlib"

SimpleInt: with {
  +: (%, %) -> %;
} == add {
  (a: %) + (b: %): % == a;
}

define SimpleGroup: Category == with {
   +: (%, %) -> %;
   -: % -> %;
   -: (%, %) -> %;
   default {
     (a: %) - (b: %): % == a + (-b);
   }
}