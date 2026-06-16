#include "types"
#include "aldorio"
#pile

UnionSet(T: with): with
    sz: % -> MachineInteger
    create: MachineInteger -> %
    parent: % -> %
    isRoot: % -> Boolean
    id: % -> MachineInteger
== add
    Rep == Record(n: MachineInteger, sz: MachineInteger, parent: %)

    create(n: MachineInteger): % ==
        rec := [n, 1, nil]
	rec.parent := rec
	return rec
	
    setParent!(s: %, p: %): () ==
        rep(s).sz := rep(s).sz + rep(p).sz
        rep(s).parent := p

    parent(s: %): % == rep(s).parent

    isRoot(s: %): Boolean == id(s) = id(parent s)

    id(s: %): MachineInteger == rep(s).n

DisjointSetCollection(T: with): with
    make: (us: %, T) -> UnionSet T
    union!: (us: %, UnionSet T, UnionSet T) -> ()
    find: T -> UnionSet(T)
== add
    Rep == Record(idCount: MachineInteger, tbl: HashMap(T, UnionSet T))

    make(us: %, t: T): UnionSet ==
        id := rep(us).idCount + 1
	rep(us).idCount := id
        create(id)

    find(us: %, t: T): UnionSet T ==
        if (isRoot(t)) then t else find(us, parent(t))
       

    union(us: %, s1: UnionSet T, s2: UnionSet T): () ==
        s1 := find s1
	s2 := find s2
	(big, small) := if sz s1 < sz s2 then (s2, s1) else (s1, s2)
	setParent!(small, big)

