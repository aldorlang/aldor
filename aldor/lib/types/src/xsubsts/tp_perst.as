#include "types"
#include "aldorio"
#pile

--https://github.com/backtracking/ocaml-bazaar/blob/main/parray.ml
PArray(T: Type): with
== add
    Arr == PrimitiveArray T
    Diff == Record(n: MachineInteger, t: T, arr: %)
    Rep == Union(a: Arr, d: Diff)
    import from Rep

    make(n: MachineInteger, v: T) == [a == new(n, v)]

    rerootk(t: %, k: () -> ()): () ==
        rep(t) case a => k()
	rerootk() -- To be completed...


	
	   