#include "foamlib"
#pile

#library LSELF0 "lself0.ao"
import from LSELF0

SomeAttr: Category == with
SUP1(R: XGroup): SUPC R with
    if R has SomeAttr then
        0: %
== add
    if R has SomeAttr then
        0: % == never

    1: % == never
    (a: %) * (b: %): % == never
    (a: R) * (b: %): % == never
    (a: %) * (b: R): % == never

local Test1: with
    bar: SUP1 % -> %
== add
    bar(x: SUP1 %): % == never
