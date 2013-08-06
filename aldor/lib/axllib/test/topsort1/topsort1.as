-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp -laxllib
#pile

-- This file tests using extend in a declarative fashion
-- to break cycles in the type form dependency graph.

#library langlib "lang.ao"
import from langlib

export NonNegativeInteger: Type

export IndexedExponents: (A1: SetCategory) ->
    IndexedDirectProductCategory(NonNegativeInteger, A1)

export Boolean: SetCategory
extend NonNegativeInteger: SetCategory

SetCategory: Category == with
        =: (%, %) -> Boolean

IndexedDirectProductCategory(B1: SetCategory, B2: SetCategory): Category ==
        SetCategory
 
