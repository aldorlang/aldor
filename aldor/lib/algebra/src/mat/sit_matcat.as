------------------------   sit_matcat.as   -----------------------
-- Copyright (c) Marco Codutti 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995
-----------------------------------------------------------------------------

#include "algebra"

macro {
	I == MachineInteger;
	V == Vector;
}

#if ALDOC
\thistype{MatrixCategory}
\History{Marco Codutti}{12 may 95}{created}
\History{Manuel Bronstein}{1/12/1999}{redesigned}
\History{Manuel Bronstein}{19/7/2002}{added diagonal(a,n) and scalar?}
\Usage{\this~R: Category}
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{ArithmeticType} &\\
}
\Descr{\this~R is a common category for matrices of arbitrary sizes
with coefficients in R. They are $1$--indexed and whether they do
bound checking depends on each particular matrix type.}
\begin{exports}
\category{\altype{CopyableType}}\\
\category{\altype{LinearArithmeticType} R}\\
\alexp{$*$}: & (\%,V) $\to$ V & multiplication by a vector\\
\alalias{\this}{bracket}{$[]$}: & \builtin{Tuple} V $\to$ \% & create a matrix\\
                                & \altype{Generator} V $\to$ \% & \\
\alexp{apply}: & (\%,I,I) $\to$ R & extract an entry\\
               & (\%,I,I,I,I) $\to$ \% & extract a submatrix\\
               & (\%, \altype{Array} I, \altype{Array} I) $\to$ \% & \\
\alexp{colCombine!}: & (\%,R,I,R,I) $\to$ \% &
In--place linear combination of columns\\ 
                     & (\%,R,I,R,I,I,I) $\to$ \% &\\
                     & (\%,(R,R)$\to$ R,I,I) $\to$ \%\\
                     & (\%,(R,R)$\to$ R,I,I,I,I) $\to$ \%\\
\alexp{colSwap!}: & (\%,I,I) $to$ \% & Swap columns in--place\\
                  & (\%,I,I,I,I) $to$ \% &\\
\alexp{column}: & (\%,I) $\to$ V & extraction of a column\\
\alexp{columns}: & \% $\to$ \altype{Generator} V & iteration over the columns\\
\alexp{companion}: & V $\to$ \% & creates a companion matrix\\
                   & (V, R) $\to$ \% & \\
\alexp{diagonal}: & V $\to$ \% & creates a diagonal matrix\\
                  & (R,I) $\to$ \% & \\
\alexp{diagonal?}: & \% $\to$ \altype{Boolean} & test for a diagonal matrix\\
\alexp{dimensions}: & \% $\to$  (I,I) & get row and column dimensions\\
\alexp{map}: & (R $\to$ R) $\to$ V $\to$ \% & lift a mapping\\
\alexp{map}: & (R $\to$ R) $\to$ \% $\to$ \% & lift a mapping\\
\alexp{map!}: & (R $\to$ R) $\to$ \% $\to$ \% & lift a mapping\\
\alexp{numberOfColumns}: & \% $\to$ I & number of columns of the matrix\\
\alexp{numberOfRows}: & \% $\to$ I & number of rows of the matrix\\
\alexp{one}: & I $\to$ \% & identity matrix\\
\alexp{one?}: & \% $\to$ \altype{Boolean} & test for an identity matrix\\
\alexp{row}: & (\%,I) $\to$ V & extraction of a row\\
\alexp{rowCombine!}: & (\%,R,I,R,I) $\to$ \% &
In--place linear combination of rows\\ 
                     & (\%,R,I,R,I,I,I) $\to$ \% &\\
                     & (\%,(R,R)$\to$ R,I,I) $\to$ \%\\
                     & (\%,(R,R)$\to$ R,I,I,I,I) $\to$ \%\\
\alexp{rowSwap!}: & (\%,I,I) $to$ \% & Swap rows in--place\\
                  & (\%,I,I,I,I) $to$ \% &\\
\alexp{rows}: & \% $\to$ \altype{Generator} V & iteration over the rows\\
\alexp{scalar?}: & \% $\to$ \altype{Boolean} & test for a scalar matrix\\
\alexp{set!}: & (\%,I,I,R) $\to$ R & set an entry in the matrix\\
\alexp{setMatrix!}: & (\%,I,I,\%) $\to$ \% & modify a submatrix of a matrix\\
\alexp{square?}: & \% $\to$ \altype{Boolean} & test for a square matrix\\
\alexp{tensor}: & (\%, \%) $\to$ \% & tensor product\\
\alexp{transpose}: & V $\to$ \% & transpose a vector\\
\alexp{transpose}: & \% $\to$ \% & transpose a matrix\\
\alexp{transpose!}: & \% $\to$ \% & transpose a matrix in--place\\
\alexp{zero}: & (I,I) $\to$ \% & create a zero matrix\\
\alexp{zero!}: & \% $\to$ () & make all the entries zero\\
\alexp{zero?}: & \% $\to$ \altype{Boolean} & test if all entries are zero\\
\end{exports}
\begin{exports}[if R has \altype{Ring} then]
\alexp{random}: & () $\to$ \% & create a random matrix\\
                & (I,I) $\to$ \% & \\
\end{exports}
\begin{exports}[if R has \altype{DifferentialRing} then]
\alexp{wronskian}: & V $\to$ \% & Wronskian matrix\\
\end{exports}
\begin{alwhere}
I &==& \altype{MachineInteger}\\
V  &==& \altype{Vector} R\\
\end{alwhere}
\begin{exports}[if R has \altype{SerializableType} then]
\category{\altype{SerializableType}}\\
\end{exports}
#endif

define MatrixCategory(R: Join(ArithmeticType, ExpressionType)): Category ==
	Join(CopyableType, LinearArithmeticType R) with {
	if R has SerializableType then SerializableType;
	*:              (%,V R) -> V R;
#if ALDOC
\alpage{$*$}
\Usage{A \name~v}
\Signature{(\%,\altype{Vector} R)}{\altype{Vector} R}
\Params{
{\em A} & \% & a matrix\\
{\em v} & \altype{Vector} R & a vector\\
}
\Retval{Returns the vector $A v$.}
#endif
	bracket: Tuple V R -> %;
	bracket: Generator V R -> %;
#if ALDOC
\alpage{bracket}
\Usage{[$v_1,\dots,v_n$]~\\ {}[v for v in g]}
\Signatures{
$[]$: & \builtin{Tuple} \altype{Vector} R $\to$ \%\\
$[]$: & \altype{Generator} \altype{Vector} R $\to$ \%\\
}
\Params{
$v_1,\dots,v_n$ & \altype{Vector} R & vectors\\
{\em g} &
\altype{Generator} \altype{Vector} R & an iterator producing vectors\\
}
\Retval{Returns the matrix whose $\sth{i}$ column is $v_i$, respectively
the $\sth{i}$ vector generated by $g$.}
#endif
	apply:          (%,I,I) -> R;
	apply:          (%,I,I,I,I) -> %;
	apply:		(%, Array I, Array I) -> %;
#if ALDOC
\alpage{apply}
\Usage{A(n, m)\\ \name(A,n,m)\\ A(n, m, r, c)\\ \name(A,n,m,r,c)\\
A(a, b)\\ \name(A, a, b)}
\Signatures{
\name: & (\%,\altype{MachineInteger},\altype{MachineInteger}) $\to$ R\\
\name: & (\%,\altype{MachineInteger},\altype{MachineInteger},
\altype{MachineInteger},\altype{MachineInteger}) $\to$ \%\\
\name: & (\%, \altype{Array} \altype{MachineInteger},
\altype{Array} \altype{MachineInteger}) $\to$ \%\\
}
\Params{
{\em A} & \% & A matrix\\
{\em n,m,r,c} & \altype{MachineInteger} & indices\\
\emph{a,b} & \altype{Array} \altype{MachineInteger} & indices\\
}
\Retval{A(n,m) returns the entry of \emph{A} at its $\sth n$ row and $\sth m$
column, while A(n,m,r,c) returns the submatrix of \emph{A} having $A(m,n)$ in
its top--left corner and $r$ rows and $c$ columns. For more general
submatrices,
$A([a_1,\dots,a_r],[b_1,\dots,b_s])$ returns the submatrix of \emph{A}
consisting of the intersection of the rows $a_1,\dots,a_r$ and
columns $b_1,\dots,b_r$ of \emph{A}.}
#endif
	colCombine!:	 (%,R,I,R,I) -> %;
	colCombine!:	 (%,R,I,R,I,I,I) -> %;
	colCombine!:    (%,(R,R)->R,I,I) -> %;
	colCombine!:    (%,(R,R)->R,I,I,I,I) -> %;
	rowCombine!:	 (%,R,I,R,I) -> %;
	rowCombine!:	 (%,R,I,R,I,I,I) -> %;
	rowCombine!:    (%,(R,R)->R,I,I) -> %;
	rowCombine!:    (%,(R,R)->R,I,I,I,I) -> %;
#if ALDOC
\alpage{colCombine!,rowCombine!}
\altarget{colCombine!}
\altarget{rowCombine!}
\Usage{
colCombine!($A,c_1,j_1,c_2,j_2$)\\
colCombine!($A,c_1,j_1,c_2,j_2,i_1,i_2$)\\
colCombine!($A,f,j_1,j_2$)\\
colCombine!($A,f,j_1,j_2,i_1,i_2$)\\
rowCombine!($A,c_1,i_1,c_2,i_2$)\\
rowCombine!($A,c_1,i_1,c_2,i_2,j_1,j_2$)\\
rowCombine!($A,f,i_1,i_2$)\\
rowCombine!($A,f,i_1,i_2,j_1,j_2$)\\
}
\Signatures{
\name:& (\%, R, I, R, I) $\to$ \%\\
\name:& (\%, R, I, R, I, I, I) $\to$ \% \\
\name: & (\%,(R,R) $\to$ R, I, I) $\to$ \%\\
\name: & (\%, (R,R) $\to$ R, I, I, I, I) $\to$ \% \\
}
\begin{alwhere}
I &==& \altype{MachineInteger}\\
\end{alwhere}
\Params{
{\em A} & \% & A matrix\\
{\em f} & (R,R) $\to$ R & An binary operation on R\\
{\em c1}, {\em c2} & R & coefficients from R \\
{\em j1}, {\em j2} & \altype{MachineInteger} & column indices\\
{\em i1}, {\em i2} & \altype{MachineInteger} & row indices\\
}
\Descr{The $\sth{j_1}$ column (resp.~$\sth{i_1}$ row) of $A$ is replaced by
the result of applying $f$ pointwise to its $\sth{j_1}$ and $\sth{j_2}$
columns (resp.~$\sth{i_1}$ and $\sth{i_2}$ rows).
If the last 2 arguments $i_1,i_2$ (resp.~$j_1,j_2$) are present, then
this operation is applied only for rows $i_1$ to $i_2$ (resp.~columns
$j_1$ to $j_2$) inclusive.
The form with $c_1$ and $c_2$ is equivalent to the first one with
the function $f$ defined by $f(x_1,x_2) = c_1 x_1+c_2 x_2$.}
#endif
	colSwap!:	(%,I,I) -> %;
	colSwap!:	(%,I,I,I,I) -> %;
	rowSwap!:	(%,I,I) -> %;
	rowSwap!:	(%,I,I,I,I) -> %;
#if ALDOC
\alpage{colSwap!,rowSwap!}
\altarget{colSwap!}
\altarget{rowSwap!}
\Usage{
colSwap!($A,j_1,j_2$)\\
colSwap!($A,j_1,j_2,i_1,i_2$)\\
rowSwap!($A,i_1,i_2$)\\
rowSwap!($A,i_1,i_2,j_1,j_2$)\\
\Signatures{
\name:& (\%, I, I) $\to$ \%\\
\name:& (\%, I, I, I, I) $\to$ \%\\
}
\begin{alwhere}
I &==& \altype{MachineInteger}\\
\end{alwhere}
\Params{
{\em A} & \% & A matrix\\
{\em j1}, {\em j2} & \altype{MachineInteger} & column indices\\
{\em i1}, {\em i2} & \altype{MachineInteger} & row indices\\
}
\Descr{The $\sth{j_1}$ and $\sth{j_2}$ columns (resp.~$\sth{i_1}$ and
$\sth{i_2}$ rows) of $A$ are exchanged in--place.
If the last 2 arguments $i_1,i_2$ (resp.~$j_1,j_2$) are present, then
this operation is applied only for rows $i_1$ to $i_2$ (resp.~columns
$j_1$ to $j_2$) inclusive.}
#endif
	column: (%, I) -> V R;
	row: (%, I) -> V R;
#if ALDOC
\alpage{column,row}
\altarget{column}
\altarget{row}
\Usage{column(A,n)\\ row(A,n)}}
\Signature{(\%,\altype{MachineInteger})}{\altype{Vector} R}
\Params{
{\em A} & \% & A matrix\\
{\em n} & \altype{MachineInteger} & An index\\
}
\Retval{Returns the $\sth{n}$ column (resp.~row) of A as a vector.}
#endif
	columns: % -> Generator V R;
	rows: % -> Generator V R;
#if ALDOC
\alpage{columns,rows}
\altarget{columns}
\altarget{rows}
\Usage{for v in columns~A repeat \{ \dots \}\\
for v in rows~A repeat \{ \dots \} }
\Signature{\%}{\altype{Generator} \altype{Vector} R}
\Params{{\em A} & \% & A matrix\\}
\Descr{This generator yields the columns (resp.~rows) of A in succession.}
#endif
	companion:	(V R, a:R == 1) -> %;
#if ALDOC
\alpage{companion}
\Usage{\name~$[r_1,\dots,r_n]$\\ \name($[r_1,\dots,r_n],a$)}
\Signature{(\altype{Vector} R, R)}{\%}
\Params{
$r_1,\dots,r_n$ & R & Entries\\
{\em a} & R & A subdiagonal entry (optional, default is $1$)\\
}
\Retval{Returns the companion matrix
$$
\pmatrix{
0 &        &   &   &  r_1  \cr
a & \ddots &   &   &  r_2  \cr
  & \ddots &   &   &  r_3  \cr
  &        &   &   & \vdots\cr
  &        &   & a &  r_n  \cr
}
$$
}
\alseealso{\alexp{diagonal}}
#endif
	diagonal:	V R -> %;
	diagonal:	(R, I) -> %;
	diagonal?:	% -> Boolean;
	scalar?:	% -> Boolean;
	square?:	% -> Boolean;
#if ALDOC
\alpage{diagonal}
\altarget{\name?}
\altarget{scalar?}
\altarget{square?}
\Usage{\name~[$r_1,\dots,r_n$]\\ \name(r, n)\\ \name?~A\\ scalar?~A\\ square?~A}
\Signatures{
\name:  & \altype{Vector} R $\to$ \%\\
\name:  & (R, \altype{MachineInteger}) $\to$ \%\\
\name?,scalar?,square?: & \% $\to$ \altype{Boolean}\\
}
\Params{
$r, r_1,\dots,r_n$ & R & Entries\\
\emph{n} & \altype{MachineInteger} & A size\\
{\em A} & \% & A matrix\\
}
\Descr{\name([$r_1,\dots,r_n$]) returns a diagonal matrix whose diagonal
elements are $r_1,\dots,r_n$,
while \name($r,n$) returns an $n \times n$ diagonal matrix with \emph{r}
on its diagonal, and
square?(A) (resp.~\name~A and scalar?~A) return \true{} if {\em A}
is a square (resp.~diagonal and diagonal with the same entry on its
diagonal) matrix, \false{} otherwise.}
\alseealso{\alexp{companion},\alexp{one?}}
#endif
	dimensions:     % -> (I,I);
#if ALDOC
\alpage{dimensions}
\Usage{\name~A}
\Signature{\%}{(\altype{MachineInteger},\altype{MachineInteger})}
\Params   {{\em A} & \% & A matrix\\}
\Retval    {The number of rows and columns in {\em A}}
\alseealso   {\alexp{numberOfColumns},\alexp{numberOfRows}}
#endif
	map:		(R -> R) -> V R -> %;
	map:		(R -> R) -> % -> %;
	map!:		(R -> R) -> % -> %;
#if ALDOC
\alpage{map}
\altarget{\name!}
\Usage{\name~f\\\name!~f\\ \name(f)([$v_1,\dots,v_n$])\\
\name(f)(A)\\\name!(f)(A)}
\Signatures{
\name: (R $\to$ R) $\to$ \altype{Vector} R $\to$ \%\\
\name: (R $\to$ R) $\to$ \% $\to$ \%\\
\name!: (R $\to$ R) $\to$ \% $\to$ \%\\
}
\Params{
{\em f} & R $\to$ R & a map\\
{$v_i$} & R & Entries of a vector\\
{\em A} & \% & A matrix\\
}
\Descr{ \name(f)([$v_1,\dots,v_n$]) returns the square matrix
$$
\pmatrix{
v_1    & \dots &   v_n  \cr
f(v_1) & \dots & f(v_n) \cr
\vdots &        & \vdots \cr
f^{n-1}(v_1) & \dots & f^{n-1}(v_n) \cr
}
$$
while \name(f)(A) returns $f(A)$,~\ie~$f$ applied to $A$ pointwise,
and \name(f) returns either the mapping $v \to f(v)$ or $A \to f(A)$.
For matrices, \name!~does not make a copy of $A$ but modifies it in place.
}
#endif
	numberOfColumns:	% -> I;
	numberOfRows:		% -> I;
#if ALDOC
\alpage{numberOfColumns,numberOfRows}
\altarget{numberOfColumns}
\altarget{numberOfRows}
\Usage     {numberOfColumns~A\\ numberOfRows~A}
\Signature{\%}{\altype{MachineInteger}}
\Params    {{\em A} & \% & A matrix\\}
\Retval    {The number of columns (resp.~rows) in {\em A}.}
\alseealso   {\alexp{dimensions}}
#endif
	one:           I -> %;
	one?:		% -> Boolean;
#if ALDOC
\alpage{one,one?}
\altarget{one}
\altarget{one?}
\Usage{one~n\\ one?~A}
\Signatures{
one:  & \altype{MachineInteger} $\to$ \%\\
one?: & \% $\to$ \altype{Boolean}\\
}
\Params{
{\em n} & \altype{MachineInteger} & an integer\\
{\em A} & \% & A matrix\\
}
\Descr{one(n) returns an $n \times n$ identity matrix, while one?(A)
returns \true{} if {\em A} is an identity matrix, \false{} otherwise.}
\alseealso{\alexp{diagonal?},\alexp{zero},\alexp{zero?}}
#endif
	if R has Ring then {
		random: () -> %;
		random: (I,I) -> %;
#if ALDOC
\alpage{random}
\Usage{\name()\\ \name(n,m)}
\Signatures{
\name: & () $\to$ \%\\
\name: & (\altype{MachineInteger}, \altype{MachineInteger}) $\to$ \%\\
}
\Params{{\em n,m} & \altype{MachineInteger} & The dimensions of the new matrix.}
\Retval    {\name() returns a random matrix with random size, while
\name(n, m) returns a random matrix with n rows and m columns.}
#endif
	}
	set!:          (%,I,I,R) -> R;
#if ALDOC
\alpage{set!}
\Usage{\name(A, n, m, x)\\ A(n,m) := x}
\Signature{(\%,\altype{MachineInteger},\altype{MachineInteger},R)}{R}
\Params{
{\em A} & \% & A matrix\\
{\em n, m} & \altype{MachineInteger} & Indices\\
{\em c} & R & An entry\\
}
\Descr{Sets $A(n,m)$ to $c$ and returns $c$.}
#endif
	setMatrix!:	(%,I,I,%) -> %;
#if ALDOC
\alpage{setMatrix!}
\Usage{\name(A, n, m, B)}
\Signature{(\%,\altype{MachineInteger},\altype{MachineInteger},\%)}{\%}
\Params{
{\em A, B} & \% & Matrices\\
{\em n, m} & \altype{MachineInteger} & Indices\\
}
\Descr{Inserts $B$ as a submatrix of $A$ starting at
$A(n,m)$ and returns $B$.}
#endif
	tensor: (%, %) -> %;
#if ALDOC
\alpage{tensor}
\Usage{\name(A,B)}
\Signature{(\%,\%)}{\%}
\Params{ {\em A, B} & \% & Matrices\\ }
\Retval{Returns $A \otimes B$, \ie~the matrix satisfying
$(A \otimes B) (u \otimes v) = A u \otimes B v$ for all vectors $u,v$.}
#endif
	transpose:	V R -> %;
	transpose:	% -> %;
	transpose!:	% -> %;
#if ALDOC
\alpage{transpose}
\altarget{\name!}
\Usage     {\name~v\\ \name~A\\ \name!~A}
\Signatures{
\name: & \altype{Vector} R $\to$ \%\\
\name,\name!: & \% $\to$ \%\\
}
\Params{
{\em v} & \altype{Vector} R & A vector\\
{\em A} & \% & A matrix\\
}
\Retval{Return the transpose of $v$ (resp.~$A$).}
\Remarks{\name!~does not make a copy of $A$, which is therefore
replaced by its transpose. It is only applicable to square matrices.}
#endif
	if R has DifferentialRing then {
		wronskian: V R -> %;
#if ALDOC
\alpage{wronskian}
\Usage{\name~[$v_1,\dots,v_n$]}
\Signature{\altype{Vector} R}{\%}
\Params{ {$v_i$} & R & Entries of a vector\\ }
\Descr{ \name([$v_1,\dots,v_n$]) returns the square matrix
$$
\pmatrix{
v_1  & \dots & v_n  \cr
v_1' & \dots & v_n' \cr
\vdots &        & \vdots \cr
v_1^{(n-1)} & \dots & v_n^{(n-1)} \cr
}
$$
}
#endif
}
	zero:           (I, I) -> %;
	zero!:		% -> ();
	zero?:		% -> Boolean;
#if ALDOC
\alpage{zero}
\altarget{\name!}
\altarget{\name?}
\Usage     {\name(n,m)\\ \name!~A\\ \name?~A}
\Signatures{
\name:  & (\altype{MachineInteger}, \altype{MachineInteger}) $\to$ \%\\
\name!: & \% $\to$ ()\\
\name?: & \% $\to$ \altype{Boolean}\\
}
\Params{
{\em n,m} & \altype{MachineInteger} & integers\\
{\em A} & \% & A matrix\\
}
\Descr{\name(n,m) returns an $n$ by $m$ zero matrix, while
\name!(A) fills $A$ with $0$'s and \name?(A)
returns \true{} if all the entries of {\em A} are $0$, \false{} otherwise.}
\alseealso{\alexp{one},\alexp{one?}}
#endif

#if NOMOREBLOODYCOMPILERBUGS
	default {
	commutative?:Boolean	== false;
	copy:% -> %		== map((r:R):R +-> r);
	-: % -> %		== map((r:R):R +-> -r);
	minus!: % -> %		== map!((r:R):R +-> -r);
	map(f:R -> R):% -> %	== (a:%):% +-> map!(f)(copy a);
	colSwap!(a:%,i:I,j:I):%	== colSwap!(a,i,j,1,numberOfRows a);
	rowSwap!(a:%,i:I,j:I):%	== rowSwap!(a,i,j,1,numberOfColumns a);
	numberOfRows(a:%):I	== { (r,c) := dimensions a; r; }
	numberOfColumns(a:%):I	== { (r,c) := dimensions a; c; }
	tensor(a:%, b:%):%	== transpose [tensorGen(a, b)];
	(a:%)^(n:I):%		== { import from Integer; a^(n::Integer); }
	one(n:I):%		== { import from R; diagonal(1, n); }
	coerce(r:R):%		== { import from I; diagonal(r, 1); }

	colCombine! (a:%,c1:R,i1:I,c2:R,i2:I) : % ==
		colCombine! (a,c1,i1,c2,i2,1,numberOfRows a);

	colCombine! (a:%,c1:R,i1:I,c2:R,i2:I,j1:I,j2:I) : % ==
		colCombine! (a,(x:R,y:R):R +-> c1*x+c2*y, i1,i2,j1,j2);

	colCombine! (a:%,f:(R,R)->R,i1:I,i2:I) : % ==
		colCombine! (a,f,i1,i2,1,numberOfRows a);

	rowCombine! (a:%,c1:R,i1:I,c2:R,i2:I) : % ==
		rowCombine! (a,c1,i1,c2,i2,1,numberOfColumns a);

	rowCombine! (a:%,c1:R,i1:I,c2:R,i2:I,j1:I,j2:I) : % ==
		rowCombine! (a,(x:R,y:R):R +-> c1*x+c2*y, i1,i2,j1,j2);

	rowCombine! (a:%,f:(R,R)->R,i1:I,i2:I) : % ==
		rowCombine! (a,f,i1,i2,1,numberOfColumns a);

	map(f:R -> R)(v:V R):% == {
		n := #v;
		a := zero(n, n);
		for j in 1..n repeat a(1,j) := v.j;
		for i in 2..n repeat for j in 1..n repeat
			a(i, j) := f a(prev i, j);
		a;
	}

	square?(a:%):Boolean == {
		import from I;
		(n, m) := dimensions a;
		n = m;
	}

	column(a:%, j:I):V R == {
		(n, m) := dimensions a;
		assert(j > 0); assert(j <= m);
		[a(i, j) for i in 1..n];
	}

	row(a:%, i:I):V R == {
		(n, m) := dimensions a;
		assert(i > 0); assert(i <= n);
		[a(i, j) for j in 1..m];
	}

	columns(a:%):Generator V R == generate {
		import from I;
		n := numberOfColumns a;
		for i in 1..n repeat yield column(a, i);
	}

	rows(a:%):Generator V R == generate {
		import from I;
		n := numberOfRows a;
		for i in 1..n repeat yield row(a, i);
	}

	(a:%) ^ (n:Integer):% == {
		assert(n >= 0);
		assert(square? a);
		zero? n => one numberOfRows a;
		one? n => a;
		b := copy a;
		for i in 2..n repeat b := times!(b, a);
		b;
	}

	(c:R) * (a:%):% == {
		zero? c => zero dimensions a;
		one? c => a;
		c = -1 => -a;
		map((r:R):R +-> c * r)(a);
	}

	times!(c:R, a:%):% == {
		zero? c => map!((r:R):R +-> 0)(a);
		one? c => a;
		c = -1 => minus! a;
		map((r:R):R +-> c * r)(a);
	}

	companion (l:V R, a:R == 1):% == {
		import from I, R;
		n := #l;
		assert(n>0);
		o := zero(n,n);
		for i in 1..prev n repeat {
			o(i,n) := l.i;
			o(next i, i) := a;
		}
		o(n, n) := l.n;
		o;
	}

	diagonal (a:R, n:I): % == {
		assert(n>0);
		o := zero(n,n);
		for i in 1..n repeat o(i,i) := a;
		o;
	}

	diagonal (l:V R): % == {
		import from I;
		n := #l;
		assert(n>0);
		o := zero(n,n);
		for i in 1..n repeat o(i,i) := l.i;
		o;
	}

	diagonal?(a:%) : Boolean == {
		import from I, R;
		(n,m) := dimensions a;
		n ~= m => false;
		for i in 1..n repeat {
			for j in 1..m | i ~= j repeat
				~zero?(a(i,j)) => return false;
		}
		true;
	}

	scalar?(a:%):Boolean == {
		import from I, R;
		(n,m) := dimensions a;
		n ~= m => false;
		zero? n => true;
		p := a(1,1);
		for i in 1..n repeat {
			i > 1 and a(i,i) ~= p => return false;
			for j in 1..m | i ~= j repeat
				~zero?(a(i,j)) => return false;
		}
		true;
	}

	one?(a:%):Boolean == {
		import from I, R;
		(r, c) := dimensions a;
		r > 0 and c > 0 and one?(a(1, 1)) and scalar? a;
	}

	extree (a:%) : ExpressionTree == {
		import from I, R, V R, List ExpressionTree;
		(r, c) := dimensions a;
		l := [extree r, extree c];
		for v in rows a repeat l := append!(l, [extree x for x in v]);
		ExpressionTreeMatrix l;	
	}

	local tensorGen(a:%, b:%):Generator V R == generate {
		import from V R;
		for u in rows a repeat for v in rows b repeat yield tensor(u,v);
	}

	if R has DifferentialRing then {
		wronskian(v:V R):% == {
			import from R;
			map(differentiate) v;
		}
	}

	if R has Ring then {
		random():% == {
			import from I;
			random(1+random()$I mod 100, 1+random()$I mod 100);
		}
	}
	}
#endif
}

#if ALDOC
\thistype{MatrixCategory2}
\History{Manuel Bronstein}{8/9/2004}{created}
\Usage{import from \this(R, MR, S, MS)}
\Params{
\emph{R,S} & \altype{ExpressionType} & Coefficient domains\\
           & \altype{ArithmeticType} &\\
{\em MR} & \altype{MatrixCategory} R & a matrix type over R\\
{\em MS} & \altype{MatrixCategory} S & a matrix type over S\\
}
\Descr{\this(R,MR,S,MS) provides tools for lifting maps $R \to S$ to
maps $MR \to MS$.}
\begin{exports}
\alexp{map}: & (R $\to$ S) $\to$ MR $\to$ MS & Lift a mapping\\
\end{exports}
#endif

MatrixCategory2(R:Join(ExpressionType, ArithmeticType), MR:MatrixCategory R,
	S:Join(ExpressionType, ArithmeticType), MS:MatrixCategory S): with {
		map: (R -> S) -> MR -> MS;
#if ALDOC
\alpage{map}
\Usage{\name~f\\\name(f)(m)}
\Signature{(R $\to$ S) $\to$ MR}{MS}
\Params{
{\em f} & R $\to$ S & A map\\
{\em m} & MR & A matrix with entries in \emph{R}\\
}
\Descr{\name(f)(m) returns a matrix whose $\sth{(i,j)}$ entry is $f(m_{ij})$,
while \name(f) returns the mapping $m \to f(m)$.}
#endif
} == add {
	map(f:R -> S)(m:MR):MS == {
		import from MachineInteger;
		(r, c) := dimensions m;
		mm:MS := zero(r, c);
		for i in 1..r repeat for j in 1..r repeat mm(i,j) := f m(i, j);
		mm;
	}
}
