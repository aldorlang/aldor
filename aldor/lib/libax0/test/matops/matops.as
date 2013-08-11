-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testgen l 

#pile
#include "ax0"

MatrixOpSym(R:Field) : MOD  == Definition where
   Mat ==> Matrix R

   MOD ==>  with

      symmetricPart : Mat -> Mat
        ++ `rank(m)' returns the symmetric part of m

   Definition ==>  add

      symmetricPart(m:Mat) : Mat ==
        m1:=transpose m
        import from R
        inv(1+1)*(m+m1)
