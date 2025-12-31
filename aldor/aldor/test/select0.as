#include "foamlib"
#pile

foo(x: MachineInteger): MachineInteger ==
    select x in
        0 => 1
	? => x * foo(x - 1)
	never

#if 0
bar(n: MachineInteger, m: MachineInteger): () ==
    select (n, m) in
        (0, 0) => 1
	(1, ?) => 5
	(0, ?) => 3
	(?a, ?) => a
	never
#endif

#if 0

Obj(x) case (?X, ?x: X) => ...

#endif
