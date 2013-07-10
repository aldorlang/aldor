------------------------------ algebra.as ------------------------------------
--
-- Header file for libalgebra clients - version 1.0.1
--
-- Copyright (c) Manuel Bronstein 1994-2002
-- Copyright (c) INRIA 1997-2002, Version 1.0.1
-- Logiciel Algebra (c) INRIA 1997-2002, dans sa version 1.0.1
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-1997
-----------------------------------------------------------------------------

-- This allows code to check whether it is built on top of libalgebra
#assert LibraryAlgebra

#include "aldor"

-- Selection of the appropriate portable object library
#if GMP
#library algebralib "libalgebra-gmp.al"
#elseif DEBUG
#library algebralib "libalgebrad.al"
#elseif DEBUGGER
#include "debuglib"
#library algebralib "libalgebradbg.al"
#else
#library algebralib "libalgebra.al"
#endif

import from algebralib;
inline from algebralib;

