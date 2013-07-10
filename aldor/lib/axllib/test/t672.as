-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#include "axllib.as"

Symbol ==> String;
import from Symbol;
Number ==> SingleInteger;
import from Number;

{Proviso(): with
 {
  sign: (Number) -> Symbol
 }
== add
 { sign(i:Number):Symbol ==
   {i>0 ==> "Positive";
    i=0 ==> "Zero";
    i<0 ==> "Negative"}
 } -- add
} -- Proviso

