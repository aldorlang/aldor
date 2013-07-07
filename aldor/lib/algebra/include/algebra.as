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
#if BuildAlgebraLib
#else
#if GMP
#library AlgebraLib "libalgebra-gmp.al"
#elseif DEBUG
#library AlgebraLib "libalgebrad.al"
#elseif DEBUGGER
#include "debuglib"
#library AlgebraLib "libalgebradbg.al"
#else
#library AlgebraLib "libalgebra.al"
#endif
#endif

import from AlgebraLib;
inline from AlgebraLib;

