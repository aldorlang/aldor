-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#pile

#include "axllib.as"

-- This file tests miscellaneous errors caught by the scope binder.

i:SingleInteger == 1
local i: SingleInteger == 2

j:SingleInteger := 1
local j

local t: Integer == 10
t == t+1

B ==> Boolean
I ==> Integer

RecursiveAggregate(S: Type): Category == BasicType with
   children: % -> List %
   nodes: % -> List %
   leaf?: % -> B
   value: % -> S
   apply: (%,Enumeration value) -> S
   cyclic?: % -> B
   leaves: % -> List S
   distance: (%,%) -> I
   child?: (%,%) -> B
   node?: (%,%) -> B
   setchildren_!: (%,List %)->%
   setelt: (%,Enumeration value,S) -> S
   setvalue_!: (%,S) -> S


Tree(S: BasicType): RecursiveAggregate(S) with 
     tree: (S,List %) -> %
     tree: S -> %
  == add
    Rep ==> Record(value: S, args: List %)
    import from S, Rep, List %
    default t,br:%
    default s:S
    default ls:List %
    (t1:%)=(t2:%):Boolean == value t1 = value t2 and children t1 = children t2

    tree(s,ls) == per [s,ls]          -- problem, no return type
    tree(s) == per [s,[]]             -- problem, no return type

    value(t:%):S == (rep t).value
    children(t:%):List % == (rep t).args

    setchildren_!(t:%,ls:List %):% == (set!(rep t,args,ls);t)
    setvalue_!(t:%,s:S):S == set!(rep t,value,s)


x:Integer := 1
a():Integer ==
    print<<x<<newline
    local x: Integer :=2
    print<<x<<newline
    x
