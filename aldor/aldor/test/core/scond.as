#include "foamlib"
#pile

SomeAttr: Category == with
SUP1(R: SomeAttr): SomeAttr with
== add

local Test1: with
    bar: SUP1 % -> %
== add
    bar(x: SUP1 %): % == never

local Test2: SomeAttr with
    bar: SUP1 % -> %
== add
    bar(x: SUP1 %): % == never

local Test3: with
    bar: % -> %
== add
    bar(x: %): % ==
        x: SUP1 % := never
	never
