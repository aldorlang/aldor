-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
#pile

#include "axllib.as"

Float: Type == add
   import from Integer
   StoredConstant ==> Record( precision:Integer, value:% )
   local = : (%,%) ->  Boolean
   local P:StoredConstant
   f(x:%,y:%):% ==
      x=y => x
      y
