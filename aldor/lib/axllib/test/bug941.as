-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs

-- Tests macro redefinition/hiding warnings.

#include "axllib"

SI==>SingleInteger;
SI==>Integer; ----------- Redefinition

import from SI;

foo():() ==
{
   SI ==> String; ------- Hides outer definition
   SO ==> Integer;
   import from SI;
}

bar():() ==
{
   SO ==> String; ------- Doesn't hide later outer definition.
   import from SO;
}

SO ==> Float;
foo();
bar();


