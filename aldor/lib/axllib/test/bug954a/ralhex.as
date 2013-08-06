
-- Original author: Ralf Hemmecke

--> testint -DCA -DC1 -Q2
--> testint -DCA -DC2 -Q2
--> testint -DCA -DC3 -Q2
--> testint -DCB -DC1 -Q2
--> testint -DCB -DC2 -Q2
--> testint -DCB -DC3 -Q2

-- Compile this file with
--   axiomxl -DCB -DC3 -grun xxx.as

-- The output will be:
--: X==ralhex
--: Y==list(22, 7, 67)
--: !!list(22, 7, 67)!

-- All other combinations of assertions, i.e., (CA,C1),(CA,C2),(CA,C3),
-- (CB,C1), and (CB,C2) yield the desired output:
--: X==ralhex
--: Y==list(22, 7, 67)
--: !ralhex!list(22, 7, 67)!

#include "axllib.as"
import from String, List Integer;
build(X,Y) ==>{
  print << "X==" << X@String <<newline _
        << "Y==" << Y@List(Integer) << newline;
#if CA
  NAME:String := X;
#elseif CB
  NAME:String == X;
#endif
  ILIST:List Integer == Y;
}

#if C1
  build("ralhex",[22,7,67]);
#elseif C2
  N=="ralhex";
  build(N,[22,7,67]);
#elseif C3
  N:="ralhex";
  build(N,[22,7,67]);
#endif

print << "!" << NAME << "!" << ILIST <<"!" <<newline;





