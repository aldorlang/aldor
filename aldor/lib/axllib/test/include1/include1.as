-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase include

--% Include1: Assertions and conditional inclusion
--
--  The lines 1, 2, 8, 9 and the contents of incl/include0.as should appear.

#assert Asserted

#if Asserted
-- 1. This line should appear
#endif

#if Asserted
-- 2. This line should appear
#else
-- 3. This line should NOT appear
#endif

#if Unasserted
-- 4. This line should NOT appear
#elseif AlsoUnasserted
#   if Asserted
    -- 5. This line should NOT appear
#   else
    -- 6. This line should NOT appear
#   endif
-- 7. This line should NOT appear
#else
-- 8. This line should appear
#   if Asserted
    -- 9. This line should appear
#   else
    -- 10. This line should NOT appear
#   endif
#includeDir "incl"
#include "includeC.as"
#endif
