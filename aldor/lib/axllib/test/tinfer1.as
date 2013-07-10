-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase tinfer
--> testerrs
#pile

-- This file tests things in tinfer that should give good error
-- messages rather than fatal storage errors or segmentation faults.

export Type:     Type
export ->:       (Tuple Type, Tuple Type) -> Type
export Tuple:    Type -> Type

boo(teg: Type, t:Type): Type == t

#if TestErrorsToo
-- Next two should produce "0 possible meanings" errors rather than
-- storage allocation bug reports.

foo(seg: XXX,  s:Type): Type == s
baz(seg: Type, s:Type): YYY  == y


-- Check for bad category forms.
local f: (t: Type) -> with BadStuff
#endif
