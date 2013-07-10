-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -DC2 -DCA -l axllib

-- (4)  axiomxl -grun -DC2 -DCA xxx.as

-- Except for the last everything seems to work well.
-- (4), however, yields the output:

--: Pkg !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
--: :::BEGIN:::
--: bsh: 18391 Memory fault: Eine Speicherabbilddatei wurde als "core"
--: erstellt.

-------------------------------------------------------------------
#include "axllib.as"
INT ==Integer;
BT  ==BasicType;

Dom(A:BT):BT == add {
  Rep ==Record(n:Integer,a:A);
  import from Rep;
  sample:% == per [0$Integer,sample$A];
  (x:%)=(y:%):Boolean == true;
  (p:TextWriter) << (x:%):TextWriter ==
    p << "(" << rep(x).n << "," << rep(x).a << ")";
}
------------------------------------------------------------------------
Pkg: with {
  foo: () -> INT;
} == add {
  print << "Pkg !!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << newline;

  foo():INT == {
    print << ":::BEGIN:::" << newline;
#if C1
    if 0>1 then return 1;
#elseif C2
    0>1 => 1;
#endif
    TT == Dom(INT);
#if CA
    tt:TT := sample$TT;
#endif
    print << ":::END:::" << newline;
    1;
  }
}

foo()$Pkg



