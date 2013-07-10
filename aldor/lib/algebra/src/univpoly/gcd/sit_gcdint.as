--------------------------- sit_gcdint.as -------------------------------------
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Institute of Scientific Computation, ETH Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"

extend AldorInteger: UnivariateGcdRing == add {
#include "sit_gcdint0"
}

-- TEMPORARY: WEIRD 1.1.12p4 COMPILER BUG (MUST BE IN A SEPARATE FILE)
-- extend GMPInteger: UnivariateGcdRing == add {
-- #include "sit_gcdint0"
-- }
