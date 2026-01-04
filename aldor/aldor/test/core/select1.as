#include "foamlib"
#pile

inc(a: MachineInteger): PPartial MachineInteger ==
    a = 0 => failed()
    success(a-1)

pattern(n: MachineInteger)(m: MachineInteger): PPartial MachineInteger == if n = m then success(m) else failed()
eq(n: MachineInteger)(m: MachineInteger): PPartial MachineInteger == if n = m then success(m) else failed()

zero(n: MachineInteger): PPartial () == if n = 0 then success() else failed()

ack(x: MachineInteger, y: MachineInteger): MachineInteger ==
    select (x, y) in
        (zero(), ?n) => inc n
	(inc ?m, zero()) => ack(m, 1)
	(inc ?m, inc ?n) => ack(m, ack(inc m, n))
	never

sterling2(n: MachineInteger, k: MachineInteger): MachineInteger ==
    select (n, k) in
        (?n, ?n) => 1
	(?n, 0) => 0
	(0, ?n) => 0
	k * sterling2(n-1, k) + sterling2(n, k-1)
	
sterling1(n: MachineInteger, k: MachineInteger): MachineInteger ==
    select (n, k) in
        (0, 0) => 1
	(?n, 0) => 0
	(0, ?n) => 0
	(inc ?m, ?k) => n * sterling1(m, k) + sterling1(m, k-1)
	
	