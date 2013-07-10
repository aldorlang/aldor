-------------------------- sit_spfcat0.as ------------------------------
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{SmallPrimeFieldCategory0}
\History{Manuel Bronstein}{15/12/98}{created}
\Usage{\this: Category}
\Descr{\this~is the category for prime fields of the form $\ZZ / p \ZZ$
where $p \in \ZZ$ is a machine prime.}
\begin{exports}
\category{\astype{PrimeFieldCategory0}}\\
\category{\altype{SerializableType}}\\
\asexp{coerce}:
& \astype{MachineInteger} $\to$ \% & conversion to a field element\\
\asexp{discreteLogTable}:
& \builtin{Cross}(A, A, \astype{Boolean}) & discrete log table if available\\
\asexp{machine}:
& \% $\to$ \astype{MachineInteger} & conversion to a machine integer\\
\end{exports}
\begin{aswhere}
A &==& \astype{PrimitiveArray} \astype{MachineInteger}\\
\end{aswhere}
#endif

macro {
	I == MachineInteger;
	A == PrimitiveArray;
}

define SmallPrimeFieldCategory0: Category == PrimeFieldCategory0 with {
	coerce: I -> %;
	machine: % -> I;
#if ALDOC
\aspage{coerce,machine}
\astarget{coerce}
\astarget{machine}
\Usage{n::\%\\ machine~m}
\Signatures{
coerce: & \astype{MachineInteger} $\to$ \%\\
machine: & \% $\to$ \astype{MachineInteger}\\
}
\Params{
{\em m} & \% & an element of the field\\
{\em n} & \astype{MachineInteger} & a machine integer\\
}
\Retval{n::\% returns n as an element of the field and machine(m)
returns m as a \astype{MachineInteger}.}
#endif
	discreteLogTable: Cross(A I, A I, Boolean);
#if ALDOC
\aspage{discreteLogTable}
\Usage{((log, exp, ok?) := \name}
\alconstant{\builtin{Cross}(A, A, \astype{Boolean})}
\begin{aswhere}
A &==& \astype{PrimitiveArray} \astype{MachineInteger}\\
\end{aswhere}
\Retval{Returns (log, exp, ok?) such that if ok? is \true,
then exp.i is $g^i$ and log.i is $\log_g(i)$ where $g$ is a
generator of the multiplicative of the group ($g$ is stored in exp.1).}
#endif
	default {
		coerce(n:I):%	== { import from Integer; n::Integer::%; }
		machine(a:%):I	== { import from Integer; machine lift a; }

		(port:BinaryWriter) << (a:%):BinaryWriter == {
			import from I;
			port << machine a;
		}

		<< (port:BinaryReader):% == {
			import from I;
			(<< port)@I :: %;
		}

		discreteLogTable:Cross(A I, A I, Boolean) == {
			import from Boolean, I, A I;
			(new 0, new 0, false);
		}
	}
}
