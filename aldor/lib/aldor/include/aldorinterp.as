------------------------------- Aldorinterp.as -----------------------------
--
-- Optional header file for interactive use of libaldor in an interpreter loop
-- Do not include this file in source files meant to be compiled!
--
-- Copyright (c) Manuel Bronstein 1998-2001
-- Copyright (c) INRIA 2000-2001, Version 0.1.13
-- Logiciel LibAldor (c) INRIA 2000-2001, dans sa version 0.1.13
----------------------------------------------------------------------------

#if DEBUG
#library aldorgloop "aldor__gloopd.ao"
#else
#library aldorgloop "aldor__gloop.ao"
#endif

import from aldorgloop;

#include "aldorio"

