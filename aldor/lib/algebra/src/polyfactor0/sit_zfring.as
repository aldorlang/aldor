----------------------------- sit_zfring.as -------------------------------
-- Copyright (c) Manuel Bronstein 1996
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1996-97
-----------------------------------------------------------------------------

#include "algebra"

macro RR == FractionalRoot Integer;

extend AldorInteger: FactorizationRing == add {
#include "sit_zfring0"
}

-- TEMPORARY: WEIRD 1.1.12p4 COMPILER BUG (MUST BE IN A SEPARATE FILE)
-- extend GMPInteger: FactorizationRing == add {
-- #include "sit_zfring0"
-- }

