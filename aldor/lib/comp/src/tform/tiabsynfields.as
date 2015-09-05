#include "comp"
#include "aldorio"
#pile

TiAbSynFields: with
    state: Field AbState
    export from AbField AbState
== add
    import from String
    state: Field AbState == field("tposs")

