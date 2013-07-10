--------------------------- sal_fltcat.as ----------------------------------
--
-- Category for floating point systems
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro Z == MachineInteger;

#if ALDOC
\thistype{FloatType}
\History{Manuel Bronstein}{26/7/99}{created}
\Usage{\this: Category}
\Descr{\this~is the category of types representing floats.}
\begin{exports}
\category{\altype{InputType}}\\
\category{\altype{OrderedArithmeticType}}\\
\category{\altype{OutputType}}\\
\category{\altype{SerializableType}}\\
\alexp{$/$}: & (\%, \%) $\to$ \% & division\\
\alexp{coerce}: & \altype{MachineInteger} $\to$ \% & conversion to a float\\
\alexp{fraction}: & \% $\to$ \% & fractional part\\
\alexp{truncate}: & \% $\to$ \altype{AldorInteger} & truncation\\
\end{exports}
#endif

define FloatType:Category ==
	Join(OrderedArithmeticType,InputType,OutputType,SerializableType) with {
	/: (%, %) -> %;
#if ALDOC
\alpage{$/$}
\Usage{$x / y$}
\Signature{(\%,\%)}{\%}
\Params{ {\em x,y} & \% & floats\\ }
\Retval{Returns the quotient of $x$ by $y$.}
#endif
	coerce: Z -> %;
#if ALDOC
\alpage{coerce}
\Usage{n::\%}
\Signature{\altype{MachineInteger}}{\%}
\Params{ {\em n} & \altype{MachineInteger} & a machine integer\\ }
\Retval{Returns n converted to a float.}
#endif
	float: Literal -> %;
	fraction: % -> %;
	truncate: % -> AldorInteger;
#if ALDOC
\alpage{fraction,truncate}
\altarget{fraction}
\altarget{truncate}
\Usage{fraction~x\\ truncate~x}
\Signatures{
fraction: & \% $\to$ \%\\
truncate: & \% $\to$ \altype{AldorInteger}\\
}
\Params{ {\em x} & \% & a float\\ }
\Retval{truncate($x$) returns $n$ such that $n x \ge 0$ and
$|n| \le |x| < |n| + 1$, while fraction($x$) returns $x - \mbox{truncate}(x)$.}
#endif
	default {
	commutative?:Boolean		== true;
	local value(c:Character):Z	== ord(c)::Z - 48;
        local plus:Character		== { import from Z; char  43; }
        local minus:Character		== { import from Z; char  45; }
        local dot:Character		== { import from Z; char  46; }
	local E:Character		== { import from Z; char  69; }
	local e:Character		== { import from Z; char 101; }
	local ten:%			== { import from Z; 10::% }

	<< (p:TextReader):% == {
		import from Character, Boolean;
		local c:Character;
		while space?(c := << p) or c = newline repeat {};
		c = plus => read p;
		c = minus => - read p;
		c = dot => scanfrac(p, 0, false);
		~digit? c => throw SyntaxException;
		scan(p, value(c)::%);
	}

	-- the sign (+/-) has been read, spaces are allowed but no newlines
	local read(p:TextReader):% == {
		import from Character, Boolean;
		local c:Character;
		while space?(c := << p) repeat {};
		c = dot => scanfrac(p, 0, false);
		~digit? c => throw SyntaxException;
		scan(p, value(c)::%);
	}

	-- the sign (+/-) and first digit before . have been read,
	-- no spaces allowed
	-- n = value already read
	local scan(p:TextReader, n:%):% == {
		import from Z, Character, Boolean;
		local c:Character;
		while digit?(c := << p) repeat n := ten * n + value(c)::%;
		c = dot => scanfrac(p, n, true);
		c = e or c = E => {
			-- do not use exponentiation from % since it is not
			-- supported in the interactive loop (C function)
			import from BinaryPowering(%, Z);
			m:Z := << p;
			m < 0 => n / binaryExponentiation(ten, -m);
			n * binaryExponentiation(ten, m);
		}
		push!(c, p);
		n;
	}

	-- the . has been read, no spaces allowed
	-- n = value before the .
	local scanfrac(p:TextReader, n:%, allowE?:Boolean):% == {
		local c:Character;
		f:% := 0;
		ex:% := 1;
		tenth:% := 0.1;
		while digit?(c := << p) repeat {
				allowE? := true;
				ex := ex * tenth;
				f := f + value(c)::% * ex;
		}
		f := f + n;
		allowE? and (c = e or c = E) => {
			-- do not use exponentiation from % since it is not
			-- supported in the interactive loop (C function)
			import from BinaryPowering(%, Z);
			m:Z := << p;
			m < 0 => f / binaryExponentiation(ten, -m);
			f * binaryExponentiation(ten, m);
		}
		push!(c, p);
		f;
	}
	}
}
