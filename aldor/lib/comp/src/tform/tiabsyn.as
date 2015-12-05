#include "comp"
#pile

extend AbSyn: with
    state: % -> AbState
    syme: % -> Syme
    set!: (%, 'syme', Syme) -> ()
    export from 'syme'
== add
    import from Field AbState
    import from String, DepTable
    local state: Field AbState == field "state"
    local syme: Field Syme == field "syme"

    state(a: %): AbState ==
        if not field?(fields a, state) then
	    tmp := abstate()
	    fields(a).state := tmp
        fields(a).state

    syme(a: %): Syme == fields(a).syme
    set!(a: %, q: 'syme', xsyme: Syme): () == fields(a).syme := xsyme
