#include "basicmath";

define Categ : Category == with {
   GcdDomain;
   foo: % -> ();
}

extend Integer: Categ  == add {

       foo(x:%):() == {}
}

-- the correct form is:
--
-- extend Fraction(R:GcdDomain): Categ  == add {
--
extend Fraction(R:Join(GcdDomain,Categ)): Categ  == add {
   import from R;
   foo(x:%):() == {}
}

import from Integer,Fraction(Integer);

bar(r:String): Categ == {
   r = "FRAC"  => Fraction(Integer);
   Integer;
}

