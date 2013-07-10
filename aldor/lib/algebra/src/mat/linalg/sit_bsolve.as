------------------------------ sit_bsolve.as --------------------------------
-- Copyright (c) Thom Mulders 1996
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1996
-----------------------------------------------------------------------------

#include "algebra"

macro {
	Z == MachineInteger;
	ARR == PrimitiveArray;
	Sn == Permutation n;
	V == Vector;
}

#if ALDOC
\thistype{Backsolve}
\History{Thom Mulders}{8 July 96}{created}
\Usage{import from \this(R,M)}
\Params{
{\em R} & \astype{IntegralDomain} & A coefficient ring\\
{\em M} & \astype{MatrixCategory} R& A matrix type over R\\
}
\Descr{\name~(R,M) provides operations for backsolving triangular systems}
\begin{exports}
\asexp{backsolve}:
& (M,Z $\to$ Z,ARR Z,Z,Z,R) $\to$ V R & Backsolve a triangular system\\
\asexp{backsolve}:
& (M,Z $\to$ Z,ARR Z,Z,M,R) $\to$ (M, V R) & Backsolve a triangular system\\
\end{exports}
\begin{exports}[if R has \astype{GcdDomain} then]
\asexp{backsolve}:
& (M,Z $\to$ Z,ARR Z,Z,Z) $\to$ V R & Backsolve a triangular system\\
\asexp{backsolve}:
& (M,Z $\to$ Z,ARR Z,Z,M) $\to$ (M, V R) & Backsolve a triangular system\\
\asexp{backsolve2}:
& (M,Z $\to$ Z,ARR Z,Z,Z,R) $\to$ V R & Backsolve a triangular system\\
\asexp{backsolve2}:
& (M,Z $\to$ Z,ARR Z,Z,M,R) $\to$ (M, V R) & Backsolve a triangular system\\
\end{exports}
\begin{aswhere}
Z &==& \astype{MachineInteger}\\
ARR &==& \astype{PrimitiveArray}\\
V &==& \astype{Vector}\\
\end{aswhere}
#endif

Backsolve(R:IntegralDomain,M:MatrixCategory R): with {
	backsolve: (M,Z->Z,ARR Z,Z,Z,R) -> V R;
	backsolve: (M,Z->Z,ARR Z,Z,M,R) -> (M, V R);
#if ALDOC
\aspage{backsolve}
\Usage{\name(a,p,st,r,c)\\ \name(a,p,st,r,b)\\
\name(a,p,st,r,c,d)\\ \name(a,p,st,r,b,d)}
\Signatures{
\name:& (M,Z $\to$ Z,\astype{PrimitiveArray} Z,Z,Z) $\to$ \astype{Vector} R\\
\name:& (M,Z $\to$ Z,\astype{PrimitiveArray} Z,Z,M) $\to$ (M,\astype{Vector} R)\\
\name:& (M,Z $\to$ Z,\astype{PrimitiveArray} Z,Z,Z,R) $\to$ \astype{Vector} R\\
\name:& (M,Z $\to$ Z,\astype{PrimitiveArray} Z,Z,M,R)$\to$(M,\astype{Vector} R)\\
}
\Params{
{\em a} & M & A matrix representing a Row Echelon Form (REF)\\
{\em p} & Z $\to$ Z & A permutation of the rows of {\em a}\\
{\em st} & \astype{PrimitiveArray} Z & The stairs of the REF\\
{\em r} & Z & The number of leading columns (before the $\sth{c}$ column)\\
{\em c} & Z & The column to be backsolved\\
{\em b} & M & A matrix whose columns have to be backsolved\\
{\em d} & R & A maximal denominator needed for a dependence relation\\
}
\begin{aswhere}
Z &==& \astype{MachineInteger}\\
\end{aswhere}
\Descr{
Backsolves a triangular system.
The triple $(a,p,st)$ represents a matrix in REF:
for $j\ge st(i)$ entry (i,j) of the REF is stored in $a(p(i),j)$.
The parameter $d$ may be omitted if $R$ is has \astype{GcdDomain},
in which case the system is solved in a minimal way. Otherwise,
$d$ must be such that $d$ times the $c$-th column of the REF
(resp.~$d$ times the columns of $b$) is a linear combination of the
first $r$ leading columns of the REF (the $\sth{j}$ column is called
leading if $j=st(i)$ for some $i$).\\
}
\Retval{
\name(a,p,st,r,c) returns a primitive vector $v$ such that $av=0$.\\
\name(a,p,st,r,c,d) returns a vector $v$ such that $av=0$,
the $\sth{c}$ coordinate of $v$ is $d$ and otherwise $v(j)\neq 0$
only if $j\le r$ and the $j$-th column is leading.\\
\name(a,p,st,r,b) returns a matrix $s$ and a vector $t$
such that when $t(l)\neq 0$, then
$a$ times the $\sth{l}$ column of $s$ equals $t(l)$ times the $\sth{l}$
column of $b$,
and when $t(l)=0$, then the $\sth{l}$ column of $b$ is not a linear
combination of the columns of $a$. $s(j,l)\neq 0$ only if the $\sth{j}$ column
is leading. Furthermore the gcd of all the entries in the $\sth{l}$ column
of $s$ and $t(l)$ is $1$.\\
\name(a,p,st,r,b,d) returns a matrix $s$ and a vector $t$
such that when $t(l)\neq 0$, then
$a$ times the $\sth{l}$ column of $s$ equals $d$ times the $\sth{l}$ column of
$b$, and when $t(l)=0$, then the $\sth{l}$ column of $b$ is not a linear
combination of the columns of $a$. $s(j,l)\neq 0$ only if the $\sth{j}$ column
is leading.
}
#endif
	if R has GcdDomain then {
		backsolve: (M,Z->Z,ARR Z,Z,Z) -> V R;
		backsolve: (M,Z->Z,ARR Z,Z,M) -> (M,V R);
		backsolve2: (M,Z->Z,ARR Z,Z,Z,R) -> V R;
		backsolve2: (M,Z->Z,ARR Z,Z,M,R) -> (M,V R);
#if ALDOC
\aspage{backsolve2}
\Usage{\name(a,p,st,r,c,d)\\ \name(a,p,st,r,b,d)}
\Signatures{
\name:& (M,Z $\to$ Z,\astype{PrimitiveArray} Z,Z,Z,R) $\to$ \astype{Vector} R\\
\name:& (M,Z$\to$ Z,\astype{PrimitiveArray} Z,Z,M,R)$\to$(M,\astype{Vector} R)\\
}
\Params{
{\em a} & M & A matrix representing a Row Echelon Form (REF)\\
{\em p} & Z $\to$ Z & A permutation of the rows of {\em a}\\
{\em st} & \astype{PrimitiveArray} Z & The stairs of the REF\\
{\em r} & Z & The number of leading columns (before the $\sth{c}$ column)\\
{\em c} & Z & The column to be backsolved\\
{\em b} & M & A matrix whose columns have to be backsolved\\
{\em d} & R & A maximal denominator needed for a dependence relation\\
}
\begin{aswhere}
Z &==& \astype{MachineInteger}\\
\end{aswhere}
\Descr{
Backsolves a triangular system in a minimal way.
The triple $(a,p,st)$ represents a matrix in REF:
for $j\ge st(i)$ entry (i,j) of the REF is stored in $a(p(i),j)$.
The parameter $d$
must be such that $d$ times the $c$-th column of the REF
(resp.~$d$ times the columns of $b$) is a linear combination of the
first $r$ leading columns of the REF (the $\sth{j}$ column is called
leading if $j=st(i)$ for some $i$).\\
}
\Retval{
\name(a,p,st,r,c,d) returns a primitive vector $v$ such that $av=0$,
and $v(j) \ne 0$ only if $j = c$ or $j \le r$ and the $\sth{j}$ column
is leading.\\
\name(a,p,st,r,b,d) returns a matrix $s$ and a vector $t$
such that when $t(l)\neq 0$, then
$a$ times the $\sth{l}$ column of $s$ equals $t(l)$
times the $\sth{l}$ column of
$b$, and when $t(l)=0$, then the $\sth{l}$ column of $b$ is not a linear
combination of the columns of $a$. $s(j,l)\neq 0$ only if the $\sth{j}$ column
is leading.
}
#endif
	}
} == add {
	backsolve(a:M,p:Z->Z,st:ARR Z,r:Z,c:Z,d:R): V R == {
		m := numberOfColumns a;
		v := zero m;
		v(c) := d;
		for i in r..1 by -1 repeat {
			e := -d * a(p(i),c);
			for j in next(i)..r repeat
				e := e - a(p(i),st(j)) * v(st(j));
			v(st(i)) := quotient(e,a(p(i),st(i)));
		}
		v;
	}

	backsolve(a:M,p:Z->Z,st:ARR Z,r:Z,b:M,d:R): (M,V R) == {
		(n, m) := dimensions a;
		s := numberOfColumns b;
		sol := zero(m,s);
		den:V R := new(s, d);
		for k in 1..s repeat {
			for j in next(r)..n repeat {
				if b(p(j),k)~=0 then { den(k) := 0; break }
			}
			if den(k)~=0 then for i in r..1 by -1 repeat {
				e := d * b(p(i), k);
				for j in next(i)..r repeat
					e := e-a(p(i),st(j))*sol(st(j),k);
				sol(st(i),k) := quotient(e,a(p(i),st(i)));
			}
		}
		(sol,den);
	}

	if R has GcdDomain then {
		local field?:Boolean == R has Field;

		backsolve(a:M,p:Z->Z,st:ARR Z,r:Z,c:Z): V R == {
			import from R,Partial R;
			field? => backsolve(a,p,st,r,c,1);
			m := numberOfColumns a;
			v := zero m;
			v(c) := 1;
			for i in r..1 by -1 repeat {
				e := -v(c)*a(p(i),c);
				for j in next(i)..r repeat
					e := e-a(p(i),st(j))*v(st(j));
				q := exactQuotient(e,a(p(i),st(i)));
				if failed? q then {
					(g,f1,f2) := gcdquo(e,a(p(i),st(i)));
					v(c) := f2*v(c);
					for j in next(i)..r repeat
						v(st(j)) := f2*v(st(j));
					v(st(i)) := f1;
				}
				else v(st(i)) := retract q;
			}
			v;
		}

		backsolve(a:M,p:Z->Z,st:ARR Z,r:Z,b:M): (M,V R) == {
			import from R,Partial R;
			field? => backsolve(a,p,st,r,b,1);
			(n, m) := dimensions a;
			s := numberOfColumns b;
			sol := zero(m,s);
			den:V R := new(s, 1);
			for k in 1..s repeat {
				for j in next(r)..n repeat
					if b(p(j),k)~=0 then {
						den(k) := 0;
						break;
					}
				if den(k)=0 then iterate;
				for i in r..1 by -1 repeat {
					e := den(k)*b(p(i),k);
					for j in i+1..r repeat
						e:=e-a(p(i),st(j))*sol(st(j),k);
					q := exactQuotient(e,a(p(i),st(i)));
					if failed? q then {
						(g,f1,f2) :=
							gcdquo(e,a(p(i),st(i)));
						den(k) := f2*den(k);
						for j in next(i)..r repeat
							sol(st(j),k) :=
								f2*sol(st(j),k);
						sol(st(i),k) := f1;
					}
					else sol(st(i),k) := retract q;
				}
			}
			(sol,den);
		}

		backsolve2(a:M,p:Z->Z,st:ARR Z,r:Z,c:Z,d:R): V R == {
			import from List R;
			field? => backsolve(a,p,st,r,c,1);
			m := numberOfColumns a;
			v := zero m;
			v(c) := d;
			for i in r..1 by -1 repeat {
				e := -d*a(p(i),c);
				for j in next(i)..r repeat
					e := e-a(p(i),st(j))*v(st(j));
				v(st(i)) := quotient(e,a(p(i),st(i)));
			}
			l:List R := [v(st(i)) for i in 1..r];
			l := cons(v(c),l);
			g := gcd generator l;
			v(c) := quotient(v(c),g);
			for i in 1..r repeat
				v(st(i)) := quotient(v(st(i)),g);
			v;
		}

		backsolve2(a:M,p:Z->Z,st:ARR Z,r:Z,b:M,d:R): (M,V R) == {
			import from List R;
			field? => backsolve(a,p,st,r,b,1);
			(n, m) := dimensions a;
			s := numberOfColumns b;
			sol := zero(m,s);
			den:V R := new(s, d);
			for k in 1..s repeat {
				for j in next(r)..n repeat
					if b(p(j),k)~=0 then {
						den(k) := 0;
						break;
					}
				if den(k)=0 then
					iterate;
				for i in r..1 by -1 repeat {
					e := d*b(p(i),k);
					for j in next(i)..r repeat
						e:=e-a(p(i),st(j))*sol(st(j),k);
					sol(st(i),k):=quotient(e,a(p(i),st(i)));
				}
				l:List R := [sol(st(i),k) for i in 1..r];
				l := cons(den(k),l);
				g := gcd generator l;
				den(k) := quotient(den(k),g);
				for i in 1..r repeat
					sol(st(i),k):= quotient(sol(st(i),k),g);
			}
			(sol,den);
		}

	}
}
