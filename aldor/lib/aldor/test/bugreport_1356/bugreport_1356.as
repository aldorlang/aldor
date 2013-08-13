-- Author: Ralf Hemmecke, Johannes Kepler Universit"at Linz
-- EMail: ralf@hemmecke.de
-- Date: 09-Feb-2004
-- Aldor version 1.0.1 for LINUX(gcc-2_96)
--   Using AldorLib from Nov 17 2003 (Bronstein)
-- Subject: [7]Braces and parentheses

-- Compilation with
--   aldor -fx -laldor -Cruntime=foam,m xxx.as
-- yields an executable which produces the following output.
-- Note that there is a newline missing.
--: foo(1,2)=2aaa
--: foo{2,3}=3bbb
--: bar{3,5}=4bar(3,5)=4

-- With the additional option -DC1 the compiler complains
--:         stdout << "foo{2,3}=" << foo{2,3} << "bbb" << newline;
--: ..........................................^
--: [L14 C43] #1 (Error) Argument 1 of `<<' did not match any possible parameter type.
--:     The rejected type is TextWriter -> TextWriter.
--:     Expected one of:
--:       -- TextWriter
--:       -- BinaryWriter

#include "aldor"
#include "aldorio"

macro I == MachineInteger;

foo(x: I, y: I): I == x+1;
bar{x: I, y: I} : I == x+1;

main():() == {
	import from I;
	stdout << "foo(1,2)=" << foo(1,2) << "aaa" << newline;
#if C1
	stdout << "foo{2,3}=" << foo{2,3} << "bbb" << newline;
#else
	stdout << "foo{2,3}=" << (foo{2,3}) << "bbb" << newline;
#endif
	stdout << "bar{3,5}=" << bar{3,5} << newline;
	stdout << "bar(3,5)=" << bar(3,5) << newline;
}
main();
