----------------------------- sit_fftring.as --------------------------------
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{FFTRing}
\History{Manuel Bronstein}{29/7/98}{created}
\Usage{\this: Category}
\Descr{\this~is the category of rings that export an algorithm for
the FFT product of univariate polynomials over themselves.}
\begin{exports}
\category{\astype{CommutativeRing}}\\
\asexp{fft}: &
(P:POL \%) $\to$ (P, P) $\to$ \astype{Partial} P & FFT product\\
\asexp{fft!}:& (P:POL \%) $\to$ (P, P, P) $\to$ \astype{Boolean} & FFT product\\
\asexp{fftCutoff}: & $\to$ \astype{MachineInteger} & Cutoff for FFT in $R[x]$\\
\end{exports}
\begin{aswhere}
POL &==& \astype{UnivariatePolynomialAlgebra0}\\
\end{aswhere}
#endif

define FFTRing: Category == CommutativeRing with {
	fft: (P: UnivariatePolynomialAlgebra0 %) -> (P, P) -> Partial P;
#if ALDOC
\aspage{fft}
\Usage{ \name(P)(p,q) }
\Signature{
(P: \astype{UnivariatePolynomialAlgebra0} \%)}
{(P, P) $\to$ \astype{Partial} P}
\Params{
{\em P} & \astype{UnivariatePolynomialAlgebra0} \% & A polynomial type\\
{\em p,q} & P & Polynomials\\
}
\Retval{Returns the product $pq$ computed using FFT.}
\alseealso{\asexp{fft!}}
#endif
	fft!: (P: UnivariatePolynomialAlgebra0 %) -> (P, P, P) -> Boolean;
#if ALDOC
\aspage{fft!}
\Usage{ \name(P)(r,p,q) }
\Signature{(P: \astype{UnivariatePolynomialAlgebra0} \%)}
{(P, P, P) $\to$ \astype{Boolean}}
\Params{
{\em P} & \astype{UnivariatePolynomialAlgebra0} \% & A polynomial type\\
{\em r,p,q} & P & Polynomials\\
}
\Retval{Replaces $r$ by $r + pq$ where the product is computed using FFT.
The space occupied by the first argument $r$ is allowed to be reused.
Returns \true~if the product could not be computed by the FFT method,
\false~otherwise.
}
\alseealso{\asexp{fft}}
#endif
	fftCutoff: MachineInteger;
#if ALDOC
\aspage{fftCutoff}
\Usage{\name}
\alconstant{\astype{MachineInteger}}
\Retval{Returns $n$ such that the FFT multiplication is used
in $R[x]$ for polynomials of degree greater than or equal to $n$.}
\Remarks{If this constant is $0$, then FFT multiplication is
not used at all in $R[x]$.}
#endif
}

