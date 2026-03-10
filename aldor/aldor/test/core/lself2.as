#include "foamlib"
#pile

#library LSELF0 "lself0.ao"
import from LSELF0

SomeProperty: Category == with

XRing: Category == with
    if % has XBiModule(%) then SomeProperty

XThing: with
    XLeftModule(%)
== add
    (a: %) * (b: %): % == never
