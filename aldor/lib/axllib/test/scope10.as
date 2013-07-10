-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase scobind
#pile

#include "axllib.as"

Flotsam: with
        _+ : (%, %) -> %

    == add
        import from Integer
        _+ : (%, %) -> %       -- duplicated signature, export wins
        _- : (%, %) -> %       -- not given as a local
        _- : % -> %            -- overloaded

        normalize: % -> %      -- missing definition
        negate : % -> %        -- ditto
        plus: (%,%) -> %       -- ditto

        default x, y : %

        - (x: %): %        == normalize negate x
        (x: %) + (y: %): % == normalize plus(x,y)

