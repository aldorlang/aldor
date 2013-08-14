----------------------------- testbug.as --------------------------------
--
-- It seems that calling a type 'Test' makes it impossible to use it:
--
-- "testbug.as", line 11: A := 2::Test;
--                        ........^
-- [L11 C9] #1 (Error) There are no suitable meanings for the operator `coerce'.
--    The possible types of the left hand side are:
--           -- AldorInteger
-- 
-- Changing 'Test' to 'MyTest' makes things work ok.
-- The compiler should not reserve such a common domain name
--

#include "aldor"

Test: with { coerce: Integer -> % } == add {
  Rep == Integer;

  coerce(i:Integer):% == per i;
}

import from Integer, Test;
A := 2::Test;

