--------------------------- sit_gcdintg.as -------------------------------------
#include "algebra"

-- TEMPORARY: WEIRD 1.1.12p4 COMPILER BUG (MUST BE IN A SEPARATE FILE)
extend GMPInteger: UnivariateGcdRing == add {
#include "sit_gcdint0"
}

