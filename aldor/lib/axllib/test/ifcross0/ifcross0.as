-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testgen f
-------------------------------------------------------------------
-- Check for the if/cross/multi/return bug. Prior to version 1.1.12
-- the compiler would segfault when compiling buggedCM and buggedMM
--
-- The problem is that we are embedding multis into cross products
-- and we have to make sure that this actually happens and that we
-- don't repeat the embedding. For example, in buggedMM we embed
-- the value of the else branch Multi->Cross so we must not attempt
-- to embed the whole if statement as well. Seems obvious but ...
-------------------------------------------------------------------

#include "axllib"

SI ==> SingleInteger;

-------------------------------------------------------------------

buggedCC(flag:Boolean):Cross(SI, SI) ==
{
   import from SI;

   r:Cross(SI, SI) := (4, 8);
   s:Cross(SI, SI) := (2, 3);

   if (flag) then
      r;
   else
      s;
}

-------------------------------------------------------------------

buggedCM(flag:Boolean):Cross(SI, SI) ==
{
   import from SI;

   r:Cross(SI, SI) := (4, 8);
   s:Cross(SI, SI) := (2, 3);

   if (flag) then
      r;
   else
      (2, 3);
}

-------------------------------------------------------------------

buggedMC(flag:Boolean):Cross(SI, SI) ==
{
   import from SI;

   r:Cross(SI, SI) := (4, 8);
   s:Cross(SI, SI) := (2, 3);

   if (flag) then
      (4, 8);
   else
      s;
}

-------------------------------------------------------------------

buggedMM(flag:Boolean):Cross(SI, SI) ==
{
   import from SI;

   r:Cross(SI, SI) := (4, 8);
   s:Cross(SI, SI) := (2, 3);

   if (flag) then
      (4, 8);
   else
      (2, 3);
}

-------------------------------------------------------------------

pprint(p:Cross(SI, SI), q:Cross(SI, SI)):() ==
{
   local fst, snd:SI;

   (fst, snd) := p;
   print << "(" << fst << ", " << snd << ")" << " ==> ";
   (fst, snd) := q;
   print << "(" << fst << ", " << snd << ")" << newline;
}

-------------------------------------------------------------------

main():() ==
{
   pprint(buggedCC(true), buggedCC(false));
   pprint(buggedCM(true), buggedCM(false));
   pprint(buggedMC(true), buggedMC(false));
   pprint(buggedMM(true), buggedMM(false));
}

-------------------------------------------------------------------

main();

-------------------------------------------------------------------
