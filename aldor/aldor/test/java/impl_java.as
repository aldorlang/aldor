#include "foamlib"

#pile

AldorUnit: JavaAttribute == add

MyTestPackage: JavaType("aldor.test") with
    AldorUnit
    testStuff: () -> ()
==
    test(): () == add
    