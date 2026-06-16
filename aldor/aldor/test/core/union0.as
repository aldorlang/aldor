#include "foamlib"
#pile

U ==> Union(x: MachineInteger, y: MachineInteger)
V ==> Union(a: MachineInteger, b: MachineInteger)

any: PatMatch((), MachineInteger) == never

foo(): U == [x == 2]

xval(u: U): MachineInteger ==
    if (u case [x == ?]) then 1 else 0

yval(u: U): MachineInteger ==
    if (u case [y == ?]) then 1 else 0

#if 0

plus(x: %, y: %): % ==
    select (x, y) in
        (0, ?) => y
	(?, 0) => x
	(monomial(?,?), monomial(?,?)) => ..
	([?lx, :rx], [?lx, : ?rx]) => ..
	

#endif
