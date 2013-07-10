-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase abcheck
#pile

-- This file tests things that can go wrong in 'with' clauses and that
-- should be caught by abcheck.

FloatTest: with
    convert: SF -> %                -- fine
    outputFloating: () -> Void      -- fine
    outputFloating: N -> Void       -- fine
    +: %-> %                        -- fine
    1: %                            -- fine
    -: %-> %; 0: %                  -- fine: abnorm should flatten
    arbitraryPrecision              -- fine
    arbitraryExponent               -- fine

    import from SF                  -- should be fine

    if SF has Field                 -- should be fine
        then
            /: (%, %) -> %
        else
            div: (%, %) -> %
            notField                -- should give warning about being ignored
