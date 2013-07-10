------------------------------- alg_leaf.as ----------------------------------
-- Copyright Swiss Federal Polytechnic Institute Zurich, 1994
--
-- This type must be included inside "sit_extree.as" to be compiled,
-- but is in a separate file for documentation purposes.

#if ALDOC
\thistype{ExpressionTreeLeaf}
\History{Manuel Bronstein}{24/11/94}{created}
\History{Manuel Bronstein}{23/6/2003}{added infix}
\Usage{import from \this}
\Descr{\this~is a type whose elements are the leafs (atoms) of expression
trees. It provides conversions to and from the basic atomic types.}
\begin{exports}
\category{\altype{OutputType}}\\
\category{\altype{PrimitiveType}}\\
\alexp{aldor}: & (TEXT, \%) $\to$ TEXT & Conversion to \aldor code\\
\alexp{axiom}: & (TEXT, \%) $\to$ TEXT & Conversion to Axiom code\\
\alexp{boolean}: & \% $\to$ \altype{Boolean} & Conversion to a boolean\\
\alexp{boolean?}: & \% $\to$ \altype{Boolean} & Test for a boolean\\
\alexp{C}: & (TEXT, \%) $\to$ TEXT & Conversion to C code\\
\alexp{doubleFloat}:
& \% $\to$ \altype{DoubleFloat} & Conversion to a double precision float\\
\alexp{doubleFloat?}:
& \% $\to$ \altype{Boolean} & Test for a double precision float\\
\alexp{float}: & \% $\to$ \altype{Float} & Conversion to a software big float\\
\alexp{float?}: & \% $\to$ \altype{Boolean} & Test for a software big float\\
\alexp{fortran}: & TEXT, \%) $\to$ TEXT & Conversion to FORTRAN code\\
\alexp{infix}: & (TEXT, \%) $\to$ TEXT & Conversion to one-dim infix output\\
\alexp{integer}:
& \% $\to$ \altype{Integer} & Conversion to a software big integer\\ 
\alexp{integer?}:
& \% $\to$ \altype{Boolean} & Test for a software big integer\\
\alexp{leaf}: & \altype{Boolean} $\to$ \% & Conversion to a leaf\\
\alexp{leaf}: & \altype{DoubleFloat} $\to$ \% & Conversion to a leaf\\
\alexp{leaf}: & \altype{MachineInteger} $\to$ \% & Conversion to a leaf\\
\alexp{leaf}: & \altype{Integer} $\to$ \% & Conversion to a leaf\\
% \alexp{leaf}: & \altype{Float} $\to$ \% & Conversion to a leaf\\
\alexp{leaf}: & \altype{SingleFloat} $\to$ \% & Conversion to a leaf\\
\alexp{leaf}: & \altype{String} $\to$ \% & Conversion to a leaf\\
\alexp{leaf}: & \altype{Symbol} $\to$ \% & Conversion to a leaf\\
\alexp{lisp}: & (TEXT, \%) $\to$ TEXT & Conversion to Lisp code\\
\alexp{singleFloat}:
& \% $\to$ \altype{SingleFloat} & Conversion to a single precision float\\
\alexp{singleFloat?}:
& \% $\to$ \altype{Boolean} & Test for a single precision float\\
\alexp{machineInteger}:
& \% $\to$ \altype{MachineInteger} & Conversion to a machine integer\\
\alexp{machineInteger?}:
& \% $\to$ \altype{Boolean} & Test for a machine integer\\
\alexp{maple}: & (TEXT, \%) $\to$ TEXT & Conversion to Maple code\\
\alexp{string}: & \% $\to$ \altype{String} & Conversion to a string\\
\alexp{string?}: & \% $\to$ \altype{Boolean} & Test for a string\\
\alexp{symbol}: & \% $\to$ \altype{Symbol} & Conversion to a symbol\\
\alexp{symbol?}: & \% $\to$ \altype{Boolean} & Test for a symbol\\
\alexp{tex}: & (TEXT, \%) $\to$ TEXT & Conversion to \LaTeX\\
\alexp{texParen?}: & \% $\to$ \altype{Boolean} & Check whether to parenthetize\\
\end{exports}
\begin{aswhere}
TEXT &==& \altype{TextWriter}\\
\end{aswhere}
#endif

ExpressionTreeLeaf: Join(OutputType, PrimitiveType) with {
	aldor:		(TEXT, %) -> TEXT;
	axiom:		(TEXT, %) -> TEXT;
	C:		(TEXT, %) -> TEXT;
	fortran:	(TEXT, %) -> TEXT;
	infix:		(TEXT, %) -> TEXT;
	lisp:		(TEXT, %) -> TEXT;
	maple:		(TEXT, %) -> TEXT;
	tex:		(TEXT, %) -> TEXT;
#if ALDOC
\alpage{aldor,axiom,C,fortran,infix,lisp,maple,tex}
\altarget{aldor}
\altarget{axiom}
\altarget{C}
\altarget{fortran}
\altarget{infix}
\altarget{lisp}
\altarget{maple}
\altarget{tex}
\Usage{{\em format}(p, a)}
\Signature{(\altype{TextWriter}, \%)}{\altype{TextWriter}}
\Params{
{\em p} & \altype{TextWriter} & The port to write to\\
{\em a} & \% & A leaf\\
}
\Descr{Writes to $p$ the expression corresponding to the leaf $a$
in the requested format.}
#endif
	boolean:	% -> Boolean;
	boolean?:	% -> Boolean;
#if ALDOC
\alpage{boolean}
\altarget{\name?}
\Usage{ \name~a\\ \name?~a }
\Signatures{
\name: & \% $\to$ \altype{Boolean}\\
\name?: & \% $\to$ \altype{Boolean}\\
}
\Params{ {\em a} & \% & A leaf\\ }
\Retval{
\name~a returns the value of $a$ as a \altype{Boolean}
if that is the type of $a$.\\
\name?~a returns \true~if a is a \altype{Boolean}, \false~otherwise.
}
#endif
	doubleFloat:	% -> DoubleFloat;
	doubleFloat?:	% -> Boolean;
#if ALDOC
\alpage{doubleFloat}
\altarget{\name?}
\Usage{ \name~a\\ \name?~a }
\Signatures{
\name: & \% $\to$ \altype{DoubleFloat}\\
\name?: & \% $\to$ \altype{Boolean}\\
}
\Params{ {\em a} & \% & A leaf\\ }
\Retval{
\name~a returns the value of $a$ as a \altype{DoubleFloat}
if that is the type of $a$.\\
\name?~a returns \true~if a is a \altype{DoubleFloat}, \false~otherwise.
}
#endif
#if GMP
	float:		% -> Float;
#endif
	float?:		% -> Boolean;
#if ALDOC
\alpage{float}
\altarget{\name?}
\Usage{ \name~a\\ \name?~a }
\Signatures{
\name: & \% $\to$ \altype{Float}\\
\name?: & \% $\to$ \altype{Boolean}\\
}
\Params{ {\em a} & \% & A leaf\\ }
\Retval{
\name~a returns the value of $a$ as a \altype{Float}
if that is the type of $a$.\\
\name?~a returns \true~if a is a \altype{Float}, \false~otherwise.
}
#endif
	integer:	% -> Integer;
	integer?:	% -> Boolean;
#if ALDOC
\alpage{integer}
\altarget{\name?}
\Usage{ \name~a\\ \name?~a }
\Signatures{
\name: & \% $\to$ \altype{Integer}\\
\name?: & \% $\to$ \altype{Boolean}\\
}
\Params{ {\em a} & \% & A leaf\\ }
\Retval{
\name~a returns the value of $a$ as an \altype{Integer}
if that is the type of $a$.\\
\name?~a returns \true~if a is an \altype{Integer}, \false~otherwise.
}
#endif
	leaf:		Boolean -> %;
	leaf:		DoubleFloat -> %;
#if GMP
	leaf:		Float -> %;
#endif
	leaf:		Integer -> %;
	leaf:		MachineInteger -> %;
	leaf:		SingleFloat -> %;
	leaf:		String -> %;
	leaf:		Symbol -> %;
#if ALDOC
\alpage{leaf}
\Usage{\name~a}
\Signatures{
\name: & \altype{Boolean} $\to$ \%\\
\name: & \altype{DoubleFloat} $\to$ \%\\
\name: & \altype{Integer} $\to$ \%\\
\name: & \altype{Float} $\to$ \%\\
\name: & \altype{MachineInteger} $\to$ \%\\
\name: & \altype{SingleFloat} $\to$ \%\\
\name: & \altype{String} $\to$ \%\\
\name: & \altype{Symbol} $\to$ \%\\
}
\Params{
{\em a} & \altype{Boolean} & A constant\\
& \altype{DoubleFloat} &\\
& \altype{Float} &\\
& \altype{Integer} &\\
& \altype{MachineInteger} &\\
& \altype{SingleFloat} &\\
& \altype{String} &\\
& \altype{Symbol} &\\
}
\Retval{\name~a returns $a$ as a leaf.}
\Remarks{A string leaf prints with quotes,
and should be used for string constants, while a symbol leaf prints without
quotes, and should be used for names.}
#endif
	negate:		% -> %;
#if ALDOC
\alpage{negate}
\Usage{\name~a}
\Signature{\%}{\%}
\Params{ {\em a} & \% & A leaf\\ }
\Retval{Returns the leaf $-a$ if $a$ is a numerical leaf, $a$ otherwise.}
\alseealso{\alexp{negative?}}
#endif
	negative?:	% -> Boolean;
#if ALDOC
\alpage{negative?}
\Usage{\name~a}
\Signature{\%}{\altype{Boolean}}
\Params{ {\em a} & \% & A leaf\\ }
\Retval{Returns \true~if $a$ is a numerical leaf and $a < 0$, \false~otherwise.}
\alseealso{\alexp{negate}}
#endif
	machineInteger:	% -> MachineInteger;
	machineInteger?:	% -> Boolean;
#if ALDOC
\alpage{machineInteger}
\altarget{\name?}
\Usage{ \name~a\\ \name?~a }
\Signatures{
\name: & \% $\to$ \altype{MachineInteger}\\
\name?: & \% $\to$ \altype{Boolean}\\
}
\Params{ {\em a} & \% & A leaf\\ }
\Retval{
\name~a returns the value of $a$ as a \altype{MachineInteger}
if that is the type of $a$.\\
\name?~a returns \true~if a is a \altype{MachineInteger}, \false~otherwise.
}
#endif
	singleFloat:	% -> SingleFloat;
	singleFloat?:	% -> Boolean;
#if ALDOC
\alpage{singleFloat}
\altarget{\name?}
\Usage{ \name~a\\ \name?~a }
\Signatures{
\name: & \% $\to$ \altype{SingleFloat}\\
\name?: & \% $\to$ \altype{Boolean}\\
}
\Params{ {\em a} & \% & A leaf\\ }
\Retval{
\name~a returns the value of $a$ as a \altype{SingleFloat}
if that is the type of $a$.\\
\name?~a returns \true~if a is a \altype{SingleFloat}, \false~otherwise.
}
#endif
	string:		% -> String;
	string?:	% -> Boolean;
#if ALDOC
\alpage{string}
\altarget{\name?}
\Usage{ \name~a\\ \name?~a }
\Signatures{
\name: & \% $\to$ \altype{String}\\
\name?: & \% $\to$ \altype{Boolean}\\
}
\Params{ {\em a} & \% & A leaf\\ }
\Retval{
\name~a returns the value of $a$ as a \altype{String}
if that is the type of $a$.\\
\name?~a returns \true~if a is a \altype{String}, \false~otherwise.
}
#endif
	symbol:		% -> Symbol;
	symbol?:	% -> Boolean;
#if ALDOC
\alpage{symbol}
\altarget{\name?}
\Usage{ \name~a\\ \name?~a }
\Signatures{
\name: & \% $\to$ \altype{Symbol}\\
\name?: & \% $\to$ \altype{Boolean}\\
}
\Params{ {\em a} & \% & A leaf\\ }
\Retval{
\name~a returns the value of $a$ as a \altype{Symbol}
if that is the type of $a$.\\
\name?~a returns \true~if a is a \altype{Symbol}, \false~otherwise.
}
#endif
	texParen?:	% -> Boolean;
#if ALDOC
\alpage{texParen?}
\Usage{\name~a}
\Signature{\%}{\altype{Boolean}}
\Params{ {\em a} & \% & A leaf\\ }
\Retval{Returns \true~if the leaf $a$ should be parenthetized,
\false~otherwise.}
#endif
} == add {
	macro {
		Rep == Union(ubool: Boolean, usint: MachineInteger,
				usfl: SingleFloat, udblf: DoubleFloat,
#if GMP
				ufloat: Float,
#endif
				uint: Integer, ustr: String, usym: Symbol);
	}

	import from Rep;

	sample:%			== leaf(1@MachineInteger);
	leaf(n:MachineInteger):%	== per [n];
	leaf(x:SingleFloat):%		== per [x];
	leaf(x:DoubleFloat):%		== per [x];
	leaf(n:Integer):%		== per [n];
	leaf(b:Boolean):%		== per [b];
	leaf(s:String):%		== per [s];
	leaf(s:Symbol):%		== per [s];
	machineInteger?(l:%):Boolean	== rep(l) case usint;
	integer?(l:%):Boolean		== rep(l) case uint;
	doubleFloat?(l:%):Boolean	== rep(l) case udblf;
	singleFloat?(l:%):Boolean	== rep(l) case usfl;
	boolean?(l:%):Boolean		== rep(l) case ubool;
	boolean(l:%):Boolean		== { assert(boolean? l); rep(l).ubool;}
	integer(l:%):Integer		== { assert(integer? l); rep(l).uint; }
	(p:TEXT) << (l:%):TEXT		== str(p, l, "_"", "_"");
	tex(p:TEXT, l:%):TEXT		== str(p, l, "``", "''");
	maple(p:TEXT, l:%):TEXT		== str(p, l, "`", "`");
	axiom(p:TEXT, l:%):TEXT		== p << l;
	aldor(p:TEXT, l:%):TEXT		== p << l;
	infix(p:TEXT, l:%):TEXT		== p << l;
	fortran(p:TEXT, l:%):TEXT	== C(p, l);
	texParen?(l:%):Boolean		== false;
	string(l:%):String		== { assert(string? l); rep(l).ustr; }
	string?(l:%):Boolean		== rep(l) case ustr;
	symbol(l:%):Symbol		== { assert(symbol? l); rep(l).usym; }
	symbol?(l:%):Boolean		== rep(l) case usym;
#if GMP
	leaf(x:Float):%			== per [x];
	float(l:%):Float		== { assert(float? l); rep(l).ufloat; }
	float?(l:%):Boolean		== rep(l) case ufloat;
#else
	local float(l:%):MachineInteger	== never;
	float?(l:%):Boolean		== false;
#endif

	doubleFloat(l:%):DoubleFloat == {
		assert(doubleFloat? l);
		rep(l).udblf;
	}

	singleFloat(l:%):SingleFloat == {
		assert(singleFloat? l);
		rep(l).usfl;
	}

	machineInteger(l:%):MachineInteger == {
		assert(machineInteger? l);
		rep(l).usint;
	}

	C(p:TEXT, l:%):TEXT == {
		boolean? l => {
			boolean l => p << "1";
			p << "0";
		}
		p << l;
	}

	lisp(p:TEXT, l:%):TEXT == {
		boolean? l => {
			boolean l => p << "t";
			p << "nil";
		}
		p << l;
	}

	(a:%) = (b:%):Boolean == {
		integer? a => integer? b and integer a = integer b;
		machineInteger? a =>
			machineInteger? b and machineInteger a=machineInteger b;
		float? a => float? b and float a = float b;
		doubleFloat? a =>
			doubleFloat? b and doubleFloat a = doubleFloat b;
		singleFloat? a =>
			singleFloat? b and singleFloat a = singleFloat b;
		string? a => string? b and string a = string b;
		symbol? a => symbol? b and symbol a = symbol b;
		boolean? a and boolean? b and boolean a = boolean b;
	}

	local str(p:TEXT, l:%, opq:String, clq:String):TEXT == {
		machineInteger? l => p << machineInteger l;
		singleFloat? l => p << singleFloat l;
		doubleFloat? l => p << doubleFloat l;
		integer? l => p << integer l;
		float? l => p << float l;
		boolean? l => p << boolean l;
		symbol? l => p << symbol l;
		p << opq << string l << clq;
	}

	negative?(l:%):Boolean == {
		machineInteger? l => machineInteger l < 0;
		doubleFloat? l => doubleFloat l < 0;
		singleFloat? l => singleFloat l < 0;
		integer? l => integer l < 0;
		float? l => float l < 0;
		false;
	}

	negate(l:%):% == {
		machineInteger? l => leaf(- machineInteger l);
		doubleFloat? l => leaf(- doubleFloat l);
		singleFloat? l => leaf(- singleFloat l);
		integer? l => leaf(- integer l);
		float? l => leaf(- float l);
		l;
	}
}
