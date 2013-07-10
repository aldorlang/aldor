--------------------------- sit_spf.as --------------------------------
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"

macro Z == MachineInteger;

#if ALDOC
\thistype{SmallPrimeField}
\History{Manuel Bronstein}{6/7/94}{created}
\Usage{import from \this~p}
\Params{ {\em p} & \altype{MachineInteger} & The characteristic\\ }
\Descr{\this~p implements the finite field $\ZZ / p \ZZ$ where $p\in \ZZ$
is a word--size prime.}
\begin{exports}
\category{\altype{SmallPrimeFieldCategory}}\\
\end{exports}
#endif

-- TEMPORARY: WORKAROUND FOR BUG1167
-- SmallPrimeField(p:Z): SmallPrimeFieldCategory == SmallPrimeField0 p;
SmallPrimeField(p:Z): SmallPrimeFieldCategory == SmallPrimeField0 p add;

#if ALDORTEST
---------------------- test sit__spf.as --------------------------
#include "algebra"
#include "aldortest"

macro F == SmallPrimeField 10007;

import from MachineInteger, F;

local inverse():Boolean == {
	a:F := 0;
	while zero? a repeat a := random();
	b := inv a;
	a * b = 1;
}

local exponentiate():Boolean == {
	import from Integer;
	a:F := 0;
	while zero? a repeat a := random();
	b := lift random();
	c:F := 1;
	for i in 1..b repeat c := c * a;
	a^b = c;
}

local sum():Boolean == {
	import from Integer;
	a:F := 0;
	while zero? a repeat a := random();
	b0:F := random();
	b := lift b0;
	c:F := 0;
	for i in 1..b repeat c := c + a;
	b0 * a = c;
}

stdout << "Testing sit__spf..." << endnl;
aldorTest("sum", sum);
aldorTest("inverse", inverse);
aldorTest("exponentiate", exponentiate);
stdout << endnl;
#endif

