-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Cut-down version of bug1192.

--> testerrs


#include "axllib"

DOM ==> Generator %;
F(u) ==> step! u;

define ACat: Category == with
{
   default
   {
      pun(U:DOM): () == {}
      bar(Y:%): () == {}
   }
}

define BCat: Category == ACat with
{
   default
   {
      foo(U:DOM):() == F U;
      wow(Y:%):() == {}
    }
}
