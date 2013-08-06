-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs

--% Include2: Simple including errors

-- Use incl/includeB to reinclude this file. 
#includeDir "."
#includeDir "incl"
#include "includeB.as"

-- Try to include a non-existent file.
#include "nonexistent.as"

-- Elses out of context
#elseif
#else

-- Unrecognized system command in excluded lines
#if NoWay
nnn
#elif YesWay
yyy
#endif

-- No endif
#if Asserted
#else
-- End of file
