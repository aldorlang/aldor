----------------------------- sit_freemod.as ----------------------------------
-- Copyright (c) Manuel Bronstein 2001
-- Copyright (c) Marc Moreno Maza 2001
-- Copyright (c) INRIA 2001
-- Copyright (c) LIFL 2001
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{FreeModule}
\History{Manuel Bronstein/Marc Moreno Maza}{26/4/2001}
{merged from UnivariateFreeFiniteAlgebra (Sumit),
and DirectProductAsModule and DirectSumAsModule (BasicMath)}
\Usage{\this~R: Category}
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{ArithmeticType} &\\
}
\Descr{\this~is a category for free modules over an
arbitrary arithmetic system R and with respect to an arbitrary basis.
Its elements are assumed to have finite support.}
\begin{exports}
\category{\altype{FreeLinearCombinationType} R}\\
\alexp{leadingCoefficient}: & \% $\to$ R & The leading coefficient\\
\alexp{nonZeroCoefficients}:
& \% $\to$ \altype{Generator} R & Iterate over the coefficients\\
\alexp{reductum}: & \% $\to$ \% & All terms except the leading one\\
\alexp{reductum!}: & \% $\to$ \% & All terms except the leading one\\
\alexp{support}:
& \% $\to$ \altype{Generator} \builtin{Cross}(R, \%) & Make an iterator\\
\alexp{term?}: & \% $\to$ Boolean & Test for a monomial\\
\alexp{trailingCoefficient}: & \% $\to$ R & The trailing coefficient\\
\end{exports}
\begin{exports}[If R has \altype{Ring} then]
\category{\altype{Module} R}\\
\end{exports}
\begin{exports}[If R has \altype{GcdDomain} then]
\alexp{content}: & \% $\to$ R & Content\\
\alexp{primitive}: & \% $\to$ (R, \%) & Content and primitive part\\
\alexp{primitive!}: & \% $\to$ (R, \%) & Content and primitive part\\
\alexp{primitivePart}: & \% $\to$ \% & Primitive part\\
\alexp{primitivePart!}: & \% $\to$ \% & Primitive part\\
\end{exports}
\begin{exports}[If $R$ has \altype{Field} then]
\alexp{monic}: & \% $\to$ \% & Make monic\\
\alexp{monic!}: & \% $\to$ \% & Make monic\\
\end{exports}
#endif

define FreeModule(R:Join(ArithmeticType, ExpressionType)): Category ==
	FreeLinearCombinationType R with {
	if R has Ring then Module R;
        if R has GcdDomain then {
                content: % -> R;
                primitive: % -> (R, %);
                primitive!: % -> (R, %);
                primitivePart: % -> %;
                primitivePart!: % -> %;
#if ALDOC
\alpage{content,primitive,primitivePart}
\altarget{content}
\altarget{primitive}
\altarget{primitive!}
\altarget{primitivePart}
\altarget{primitivePart!}
\Usage{content~m\\ content!~m\\
primitive~m\\ primitive!~m\\
primitivePart~m\\ primitivePart!~m}
\Signatures{
content,content!: & \% $\to$ R\\
primitive,primitive!: & \% $\to$ (R, \%)\\
primitivePart,primitivePart!: & \% $\to$ \%\\
}
\Params{ {\em m} & \% & An element of the module\\ }
\Descr{content(m) returns the gcd of the coefficients of m,
while primitive(m) and primitivePart(m) return respectively
$(c, c^{-1} m)$ and $c^{-1} m$ where $c = \mbox{content}(p)$.}
\Remarks{The storage used by $m$ is allowed to be destroyed or reused
if content!, primitive! or primitivePart! is used,
so $m$ is lost after those calls.
This may cause $m$ to be destroyed, so do not use this unless
$m$ has been locally allocated, and is guaranteed not to share space
with other elements. Some functions are not necessarily copying their
arguments and can thus create memory aliases.}
#endif
	}
	term?: % -> Boolean;
#if ALDOC
\alpage{term?}
\Usage{\name~m}
\Signature{\%}{\altype{Boolean}}
\Params{ {\em m} & \% & An element of the module\\ }
\Retval{Returns \true~if $m = r e$ for $r \in R$ and $e$ an element
of the basis, \false~otherwise.}
\Remarks{\name(0) returns \true.}
#endif
	leadingCoefficient: % -> R;
	trailingCoefficient: % -> R;
#if ALDOC
\alpage{leadingCoefficient,trailingCoefficient}
\altarget{leadingCoefficient}
\altarget{trailingCoefficient}
\Usage{leadingCoefficient~m\\ trailingCoefficient~p}
\Signature{\%}{R}
\Params{ {\em m} & \% & An element of the module\\ }
\Retval{leadingCoefficient(m) and trailingCoefficient(m)
return respectively the leading and trailing coefficient of $m$.
Both return 0 when $p = 0$.}
\alseealso{\alfunc{UnivariateFreeLinearArithmeticType}{coefficients},
\alexp{nonZeroCoefficients}}
#endif
	if R has Field then {
		monic: % -> %;
		monic!: % -> %;
#if ALDOC
\alpage{monic}
\altarget{\name!}
\Usage{\name~m\\ \name!~m}
\Signature{\%}{\%}
\Params{ {\em m} & \% & An element of the module\\ }
\Retval{Returns $r^{-1} m$ where $r$ is the leading coefficient of $m$,
returns $0$ if $m = 0$.}
\Remarks{The storage used by $m$ is allowed to be destroyed or reused
if \name! is used, so $m$ is lost after this call.
This may cause $m$ to be destroyed, so do not use this unless
$m$ has been locally allocated, and is guaranteed not to share space
with other elements. Some functions are not necessarily copying their
arguments and can thus create memory aliases.}
#endif
	}
	nonZeroCoefficients: % -> Generator R;
#if ALDOC
\alpage{nonZeroCoefficients}
\Usage{for c in \name~m repeat \{ \dots \} }
\Signature{\%}{\altype{Generator} R}
\Params{ {\em m} & \% & An element of the module\\ }
\Retval{Returns a generator that produces all the nonzero coefficients of $m$.}
#endif
	reductum: % -> %;
	reductum!: % -> %;
#if ALDOC
\alpage{reductum}
\altarget{\name!}
\Usage{\name~m\\ \name!~m}
\Signature{\%}{\%}
\Params{ {\em m} & \% & An element of the module\\ }
\Retval{Returns the reductum of $m$, \ie~$p - r e$ where
$r$ is the leadingCoefficient of $p$ and $e$ the corresponding element.
Returns 0 if $m = 0$.}
\Remarks{The storage used by $m$ is allowed to be destroyed or reused
if \name! is used, so $m$ is lost after this call.
This may cause $m$ to be destroyed, so do not use this unless
$m$ has been locally allocated, and is guaranteed not to share space
with other elements. Some functions are not necessarily copying their
arguments and can thus create memory aliases.}
#endif
	support: % -> Generator Cross(R, %);
#if ALDOC
\alpage{support}
\Usage{\name~m}
\Signature{\%}{\altype{Generator} \builtin{Cross}(R, \%)}
\Params{ {\em m} & \% & An element of the module\\ }
\Descr{\name(m) generates the terms of m, \ie~the smallest number of pairs
$(r,e)$ where $e$ lies in the basis and whose sum is $m$.}
#endif
	default {
		term?(p:%):Boolean	== zero? reductum p;
		reductum!(p:%):%	== reductum p;

		nonZeroCoefficients(p:%):Generator R == generate {
			for term in support p repeat {
				(r, e) := term;
				yield r;
			}
		}

		map(f:R -> R)(p:%):% == {
			q:% := 0;
			for term in support p repeat {
				(c, n) := term;
				q := add!(q, f c, n);
			}
			q;
		}

		if R has Field then {
			-- TEMPORARY: WANT THOSE DEFS EVENTUALLY
			-- content(p:%):R		== 1;
			-- primitivePart(p:%):%	== p;
			-- primitive(p:%):(R, %)	== (1, p);

			monic(p:%):% == {
				import from R;
				zero? p or one?(a := leadingCoefficient p) => p;
				inv(a) * p;
			}

			monic!(p:%):% == {
				import from R;
				zero? p or one?(a := leadingCoefficient p) => p;
				times!(inv a, p);
			}
		}

		if R has GcdDomain then {
			local field?:Boolean	== R has Field;

			content(p:%):R == {
				field? => 1;
				gcd nonZeroCoefficients p;
			}

			primitivePart!(p:%):% == {
				field? => p;
				(c, q) := primitive! p;
				q;
			}

			primitivePart(p:%):% == {
				field? => p;
				(c, q) := primitive p;
				q;
			}

			primitive!(p:%):(R, %) == {
				import from R;
				field? => (1, p);
				one?(c := content p) => (c, p);
				(c, map!((r:R):R +-> quotient(r, c)) p);
			}

			primitive(p:%):(R, %) == {
				import from R;
				field? => (1, p);
				one?(c := content p) => (c, p);
				(c, map((r:R):R +-> quotient(r, c)) p);
			}
		}
	}
}

