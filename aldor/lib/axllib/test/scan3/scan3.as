-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase scan
#pile

--% Scan3: Exhaustive test of all forms of numbers
--
-- r = radix specifier:      36r         (2 <= radix <= 36)
-- w = whole part:           999         (or 99FFEE    with radix)
-- p = point
-- f = fraction part:        999         (or 99FFEE    with radix)
-- e = exponent part:        [eE]99      (only e99     with radix)
-- es= signed exponent part: [eE][+-]99  (only e[+-]99 with radix)

-- The various forms of numbers
.1		--   pf
.98             --   pf
.98e7           --   pfe
.98E+7          --   pfes
32              --  w   
32E7            --  w  e
32e-7           --  w  es
0.		--  wp
32.             --  wp   
32.e7           --  wp e
32.e-7          --  wp es
32.98           --  wpf
32.98e7         --  wpfe
3.9e+7          --  wpfes
15r.BE8         -- r pf
8r.70e4         -- r pfe
10r.98e+7       -- r pfes
16rFFEA         -- rw   
16rFFEAe3       -- rw  e
16rFFEAe-4      -- rw  es
17r32.          -- rwp   
2r01.e7         -- rwp e
18rAG.e+12      -- rwp es
12r12TE.TE      -- rwpf
8r76.05e9       -- rwpfe
10r9AB.DEe+987  -- rwpfes

-- Various forms of non-numbers
-- (These generate errors)
3.e-            --  wp es
100r            -- r    
100re9          -- r   e
10re+9          -- r   es
16r.            -- r p   
16r.e4          -- r p e
16r.e+4         -- r p es
99r9AB.DEe+987  -- rwpfes
-- (These generate other junk)
3.+             --  wp  s
.e7             --   p e
.e+7            --   p es
