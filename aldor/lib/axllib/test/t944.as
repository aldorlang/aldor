-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib
--> testrun -O -l axllib
--> testcomp -O

#include "axllib.as"

local int:Integer;
pos:SingleInteger:=1;
buf:String := "3xyz";
(int, pos) := scan(buf, pos)$Format;
print << "scanned integer is: " << int << newline;
print << "new buffer pos is: " << pos << newline;
