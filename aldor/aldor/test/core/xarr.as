#include "foamlib.as"
#pile

Foo: with
    new: Tuple SingleInteger -> %
== add
    Rep == List SingleInteger
    import from Rep

    new(t: Tuple SingleInteger): % == per [t]
