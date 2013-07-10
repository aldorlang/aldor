----------------------------- sit_zfringg.as -------------------------------
#include "algebra"

macro RR == FractionalRoot Integer;

-- TEMPORARY: WEIRD 1.1.12p4 COMPILER BUG (MUST BE IN A SEPARATE FILE)
extend GMPInteger: FactorizationRing == add {
#include "sit_zfring0"
}

