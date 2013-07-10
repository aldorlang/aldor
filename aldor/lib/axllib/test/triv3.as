-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--  Triv3      -- A minimal program depending on domain code.
#pile

--> testcomp
--> testgen c
--> testgen l
--> testrun -l axllib

#library AxlLib "axllib"

import from AxlLib, TextWriter, Character, String

print << "!" << newline
