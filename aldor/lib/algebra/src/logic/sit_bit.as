#include "algebra"

Bit: BooleanAlgebra with {
   coerce: Boolean -> %;
   coerce: % -> Boolean;
}
 == add {
   Rep == Boolean;
   import from Rep;
   true: % == per true;
   false: % == per false;

   true?(a: %): Boolean == rep a;
   false?(a: %): Boolean == not rep a;

   _and(a: %, b: %): % == per(rep a and rep b);
   _or(a: %, b: %): % == per(rep a or rep b);
   _not(a: %): % == per not rep a;

   coerce(b: Boolean): % == per b;
   coerce(bit: %): Boolean == rep bit;

   extree(a: %): ExpressionTree == extree rep a;

   (a: %) = (b: %): Boolean == rep a = rep b;
}