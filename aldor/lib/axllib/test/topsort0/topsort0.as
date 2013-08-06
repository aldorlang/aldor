-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp -laxllib
#pile

-- This file tests the topological sorting of type forms.
-- We should be able to detect the cycle (typeof(SetCategory), typeof(Boolean),
-- process the cycle, and then process the type forms which depend on it.

#library langlib "lang.ao"
import from langlib

export IndexedExponents: (A1: SetCategory) ->
    IndexedDirectProductCategory(NonNegativeInteger, A1)

export Boolean: SetCategory
export NonNegativeInteger: SetCategory

SetCategory: Category == with
        =: (%, %) -> Boolean

IndexedDirectProductCategory(B1: SetCategory, B2: SetCategory): Category ==
        SetCategory
 
