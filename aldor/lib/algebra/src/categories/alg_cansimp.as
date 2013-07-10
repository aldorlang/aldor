#include "algebra"

macro {
	NonNegativeInteger == Integer;
}

+++ `CanonicalSimplification' asserts that a domain of this category
+++ supports a canonical simplifier denoted by `mod'. This means that for
+++ any `p:%' such that `p = unitCanonical(p)' holds and for any `a:%'
+++ the element `a mod p' is a canonical representative of the residue
+++ class of `a' by `p'. Thus for any `b:%' the relation `a mod p = b mod p'
+++ holds iff `p' divides `a -b' . Moreover we require that `a mod p'
+++ and `unitCanonical(a mod p)' return the same element.
+++ Finally, if the domain is an ordered ring, we assume that we have a second
+++ canonical simplifier denoted  by `symetricMod' such that for any
+++ positive `p' and any `a:%' the element `2 * symetricMod(a,p)' lies
+++ in the closed segment `1-p..(p-1)'. To make the description of this
+++ category more simple, the operation `symetricMod' is declared in any
+++ case; if the domain is not an integral domain, then `symetricMod(a,p)'
+++ returns the same result as `a mod p'.
+++ WARNING: For efficiency reasons, each exported operation matching
+++ `mod' except `mod: (%, %) -> %' assumes that each of its arguments
+++ `a', but the last one `p', satisfies `a = a mod p'.
+++ Author: Marc Moreno Maza
+++ Date Created: 04/11/99
+++ Date Last Update: 24/11/03

define CanonicalSimplification: Category == CommutativeRing with {
        mod: (%, %) -> %;
          ++ `a mod p' returns a representative of the residue class of `a'
          ++ by `p'. This operation is required to be a canonical simplifier.
          ++ Thus  for any `a' and `b' in `%' and any `p' in `%' such that
          ++ `p = unitCanonical(p)' holds we have `a mod p = b mod p' iff
          ++ `p' divides `a -b'. Moreover `a mod p = unitCanonical(a mod p)'
          ++ is needed.
        mod_-: (%, %) -> %;
          ++ `mod_-(a,p)' returns `-a mod p'.
          ++ This assumes `a = a mod p'.
        mod_+: (%, %, %) -> %;
          ++ `mod_+(a,b,p)' returns `(a + b) mod p'.
          ++ This assumes `a = a mod p' and `b = b mod p'.
        mod_-: (%, %, %) -> %;
          ++ `mod_-(a,b,p)' returns `(a - b) mod p'.
          ++ This assumes `a = a mod p' and `b = b mod p'.
        mod_*: (%, %, %) -> %;
          ++ `mod_*(a,b,p)' returns `(a * b) mod p'.
          ++ This assumes `a = a mod p' and `b = b mod p'.
        -- mod_^: (%, NonNegativeInteger, %) -> %;
          -- ++ `mod_^(a,n,p)' returns `(a^n) mod p'.
          -- ++ This assumes `a = a mod p'.
        -- mod__prime_^: (%, NonNegativeInteger, %) -> %;
          -- ++ `mod__prime_^(a,n,p)' returns `(a^n) mod p'.
          -- ++ This assumes that `p' is a prime and that `a = a mod p'.
        recipMod: (%, %) -> Partial(%);
          ++ `recipMod(a,b)' returns the inverse of `a' modulo `b'.
          ++ (that is a number `c' such that `(c * a) mod b' is `1')
          ++ if such a number exists, otherwise `failed()' is returned.
          ++ This assumes `a = a mod p'.
        invMod: (%, %) -> %;
          ++ `invMod(a,b)' returns `c' if `recipMod(a,b)' returns
          ++ `[c]' otherwise produces an error.
          ++ This assumes `a = a mod p'.
	if % has EuclideanDomain then {
	    symmetricMod: (%, %) -> %;
	    ++ `symmetricMod(x,o)'  Kaltofen Monagan ISSAC 1999
	}
        default {
		mod_-(a: %, p: %): % == (-a) mod p;
		mod_-(a: %, b: %, p: %): % == (a - b) mod p;
        	mod_+(a: %, b: %, p: %): % == (a + b) mod p;
		mod_*(a: %, b: %, p: %): % == (a * b) mod p;
		-- mod__prime_^(a: %, n: NonNegativeInteger, p: %): % == mod_^(a, n, p);
		invMod(a: %, b: %): % == {
			r?: Partial(%) := recipMod(a,b);
			assert(not failed? r?);
			retract(r?);
		}
        }

}





#if ALDOC
\thistype{CanonicalSimplification}
\History{Marc Moreno Maza}{1999}{created}
\Usage{\this: Category}
\Descr{\this~is the category  of domains supporting a canonical simplifier.
This means that for any $p$ such that $p$ equals its canonical associate
and for any $a$, the element $a \ {\rm mod} \ p$ is a canonical representative of the residue
class of $a$ by $p$. That is,  for any $b$ the relation $a \ {\rm mod} \ p = b \ {\rm mod} \ p$ is
equivalent to $p$ divides $b -a$. In addition $a \ {\rm mod} \ p$ equals
its canonical associate.
If the domain is Euclidean, then it must support also a symmetric canonical simplifier.
See the paper {\em On the genericity of the Modular Gcd Algorithm}
by Kaltofen and Monagan in the proc. of ISSAC 1999.}
\begin{exports}
\category{\altype{CommutativeRing}} \\
\alexp{mod}: &  (\%, \%) $\to$ \%    &  residue class representative \\
\alexp{mod\_+}: &  (\%, \%) $\to$ \%    &  modular sum \\
\alexp{mod\_+}: &  (\%, \%) $\to$ \%    &  modular difference \\
\alexp{mod\_*}: &  (\%, \%) $\to$ \%    &  modular product \\
%% \alexp{mod\_^}: &  (\%, \altype{Integer}, \%) $\to$ \%    &  modular exponentiation \\
%% \alexp{mod\_prime\_^}: &  (\%, \altype{Integer}, \%) $\to$ \%    &  modular exponentiation with prime exponent \\
\alexp{recipMod} &  (\%, \%) $\to$ \astype{Partial} \%  & modular reciprocal \\
\end{exports}
\begin{exports}[if \% has \altype{EuclideanDomain} then]
\alexp{symmetricMod}: &  (\%, \%) $\to$ \%    & symmetric residue class representative \\
\end{exports}
#endif
