--* From adk@mailer.scri.fsu.edu  Fri Jul  4 18:47:39 1997
--* Received: from nagmx1.nag.co.uk by red.nag.co.uk via SMTP (920330.SGI/920502.SGI)
--* 	for /home/red5/axiom/support/recvbug id AA12996; Fri, 4 Jul 97 18:47:39 +0100
--* Received: from mailer.scri.fsu.edu (mailer.scri.fsu.edu [144.174.112.142])
--*           by nagmx1.nag.co.uk (8.8.4/8.8.4) with ESMTP
--* 	  id SAA05072 for <ax-bugs@nag.co.uk>; Fri, 4 Jul 1997 18:48:50 +0100 (BST)
--* Received: from ibm4.scri.fsu.edu (ibm4.scri.fsu.edu [144.174.131.4]) by mailer.scri.fsu.edu (8.8.5/8.7.5) with ESMTP id NAA22867; Fri, 4 Jul 1997 13:47:29 -0400 (EDT)
--* From: Tony Kennedy <adk@scri.fsu.edu>
--* Received: by ibm4.scri.fsu.edu (8.8.5) id RAA39944; Fri, 4 Jul 1997 17:44:32 GMT
--* Date: Fri, 4 Jul 1997 17:44:32 GMT
--* Message-Id: <199707041744.RAA39944@ibm4.scri.fsu.edu>
--* To: adk@scri.fsu.edu, ax-bugs@nag.co.uk, edwards@scri.fsu.edu
--* Subject: [5] What does this warning mean?

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -V -Fao countable.as > countable.log
-- Version: 1.1.9a
-- Original bug file name: countable.as

--+ AXIOM-XL version 1.1.9a for AIX RS/6000 
--+ "countable.as", line 215: extend Integer: CountablyInfinite Integer ==
--+                           ^
--+ [L215 C1] #1 (Warning) Couldn't find identifier for documentation
--+ 
--+ "countable.as", line 229: 
--+ extend SingleInteger: CountablyInfinite SingleInteger ==
--+ ^
--+ [L229 C1] #2 (Warning) Couldn't find identifier for documentation
--+ 
--+                ld in sc sy li pa ma ab ck sb ti gf of pb pl pc po mi
--+  Time   11.3 s  0  1 .4  1 .3  1 .1 .3 .1  2 62  6  8 18 .1  0  0 .4 %
--+ 
--+  Source  458 lines,  2425 lines per minute
--+  Lib   65533 bytes,  13685syme 30975foam 604fsyme 2909name 595kind 552file 2026lazy 9767type 2inl 826twins 48ext 3371doc 14id 3macros
--+  Store  5856 K pool
#include "axllib.as"
#pile

SI ==> SingleInteger
SINS ==> Join(Steppable, IntegerNumberSystem)

+++ `Countable' provides a one-to-one mapping to the Integer Number System I,
+++ and thus to produce a generator `all' which enumerate element of any
+++ domain belonging to this category. 
+++
+++ Author: ADK
+++ Date Created: 26-APR-1995 17:54:53.00
+++ Modifications:
+++   1-APR-1996 18:18:39.00 (ADK) Any type which is countable must at
+++                                least be a set (`BasicType'). This
+++                                means that `= : (%,%) -> Boolean' is
+++                                guaranteed to exist.

define Countable(I: SINS): Category == BasicType with
  ord: % -> I			++ Index within enumeration
  val: I -> %			++ Enumeration of countable set
  all: () -> Generator %	++ Generator which enumerates set

+++ `CountablyInfinite' provides a one-to-one mapping to the Integer Number
+++ System I, and thus to produce a generator `all' which enumerate element of
+++ any domain belonging to this category. 
+++
+++ Author: ADK
+++ Date Created: 26-APR-1995 17:54:53.00
+++ Modifications:
+++   1-APR-1996 18:18:39.00 (ADK) Added default definitions for `val' and
+++                                `ord'. Note that at least one of `all' and
+++                                `val' must be explicitly defined or these
+++                                default definitions will suffer infinite
+++                                recursion with no compile-time warning.

define CountablyInfinite(I: SINS): Category == Countable I with
  default
    all(): Generator % ==
      import from I, Segment I
      generate for i: I in 1 .. repeat yield val i

    val(i: I): % ==
      import from Segment I
      for a: % in all() for j: I in 1 .. repeat
        i = j => return a
      never

    ord(a: %): I ==
      import from Segment I
      for b: % in all() for j: I in 1 .. repeat
	a = b => return j
      never

+++ `CountablyFinite' provides a one-to-one mapping to the Integer Number
+++ System I in the range 1 .. card, and thus to produce a generator which
+++ enumerates element of any domain belonging to this category. 
+++
+++ Author: ADK
+++ Date Created: 27-APR-1995 13:09:53.00
+++ Modifications:
+++   1-APR-1996 18:18:39.00 (ADK) Added default definitions for `val' and
+++                                `ord'. Note that at least one of `all' and
+++                                `val' must be explicitly defined or these
+++                                default definitions will suffer infinite
+++                                recursion with no compile-time warning.

define CountablyFinite(I: SINS): Category == Countable I with
  card: I			++ Cardinality of domain

  default
    all(): Generator % ==
      import from I, Segment I
      generate for i: I in 1 .. card repeat yield val i

    val(i: I): % ==
      import from Segment I
      for a: % in all() for j: I in 1 .. repeat
        i = j => return a
      error "val$CountablyFinite: out of range"

    ord(a: %): I ==
      import from Segment I
      for b: % in all() for j: I in 1 .. repeat
	a = b => return j
      error "ord$CountablyFinite: out of range"

+++ `DirectProduct' produces a domain which is the direct product of two
+++ countable sets.
+++
+++ Author: ADK
+++ Date Created: 26-APR-1995 17:54:53.00
+++ Modifications:

DirectProduct(I: SINS, A: Countable I, B: Countable I): with

    if A has CountablyFinite I and B has CountablyFinite I
      then CountablyFinite I else CountablyInfinite I

    coerce: (A, B) -> %		++ constructor

    coerce: % -> Cross(A, B)	++ deconstructor

  == add

    Rep == Record(first: A, second: B)

    import from Rep, I

    (a: %) = (b: %): Boolean == (rep a) = (rep b)

    (p: TextWriter) << (a: %): TextWriter ==
      p << "DirectProduct(" << rep(a).first << "," << rep(a).second << ")"

    card: I ==
      -- if % has CountablyFinite I then -- doesn't work!!!
      if A has CountablyFinite I and B has CountablyFinite I then 
	card$A * card$B
      else -- this shouldn't have to be here!!!
	-1

    ord(a: %): I ==
      local 
	alpha: I == ord((rep a).first)
	beta: I == ord((rep a).second)
      A has CountablyFinite I => alpha + card$A * (beta - 1)
      B has CountablyFinite I => beta + card$B * (alpha - 1)      
      n * (n+1) quo 2 + alpha where n: I ==  alpha + beta - 2

    -- iSqrt(a) gives the largest b such that b*b <= a

    local iSqrt(a: I): I ==
      import from I
      local b: I == approxSqrt a
      not ((b-1)*(b-1) < a < (b+1)*(b+1)) =>
	error "iSqrt$DirectProduct: this cannot happen"
      b * b > a => b - 1
      b

    -- approxSqrt(a) returns b such that (b-1)^2 < a < (b+1)^2

    local approxSqrt(a: I): I ==
      import from I, SingleInteger
      (n := length a) > 100 =>
	 n := n quo 4
	 s := approxSqrt shift(a, -2 * n)
	 s := shift(s, n)
	 return ((1 + s + a quo s) quo 2)
      local (b, old): I := (shift(1, n quo 2), 1)
      while b ~= old repeat
	 (b, old) := ((1 + b + a quo b) quo 2, b)
      b

    val(a: I): % ==
      a < 1 => error "val$DirectProduct: out of range"
      A has CountablyFinite I => ( per [val(alpha + 1), val(beta + 1)]
	  where (beta: I, alpha: I) == divide(a - 1, card$A) )
      B has CountablyFinite I => ( per [val(alpha + 1), val(beta + 1)]
	where (alpha: I, beta: I) == divide(a - 1, card$B) )
      per [val(alpha), val(beta)] where 
	n: I == ( iSqrt(8*a - 7) - 1 ) quo 2
	alpha: I == a  -  n * (n+1) quo 2
	beta: I == n - alpha + 2

    coerce(a: A, b: B): % == per [a, b]

    sample: % == coerce (sample$A, sample$B)

    coerce(a: %): Cross(A, B) == ((rep a).first, (rep a).second)

+++ `Bounded Integer' produces a domain of integers suitable for indexing
+++ a one-dimensional array. The idea is that one gets bounds checking for
+++ free. Note the distinction between the functions `ord' and `val'
+++ (inherited from `Countable') and `coerce' for the case where low ~= 1.
+++
+++ Author: ADK
+++ Date Created: 26-APR-1995 17:54:53.00
+++ Modifications:

BoundedInteger(I: SINS, low: I, high: I): CountablyFinite I
    with
      coerce: % -> I		++ Embedding of % into I
      coerce: I -> %		++ Retraction of I into %

  == I add
    import from FormattedOutput
    Rep == I

    if low > high then error(string
      . "BoundedInteger(I, ~a, ~a) is empty, so what is sample?"
        . (<< low, << high))

    sample: % == coerce low

    card: I == high - low + 1

    ord(a: %): I == (rep a) - low + 1

    val(a: I): % ==
      a < 1 or a > card => error "val$BoundedInteger: out of range"
      per (a + low - 1)

    coerce(a: %): I == rep a

    coerce(a: I): % == 
      a < low or a > high => error "coerce$BoundedInteger: out of range"
      per a

+++ `Integer' is extended to make it belong to `Countable'
+++
+++ Author: ADK
+++ Date Created: 27-APR-1995 15:42:12.00
+++ Modifications:

extend Integer: CountablyInfinite Integer ==
  add
    Rep ==> Integer

    ord(a: %): Integer == rep a

    val(a: Integer): % == per a

+++ `SingleInteger' is extended to make it belong to `Countable'
+++
+++ Author: ADK
+++ Date Created:  4-APR-1996 11:41:49.00
+++ Modifications:

extend SingleInteger: CountablyInfinite SingleInteger ==
  add
    Rep ==> SingleInteger

    ord(a: %): SingleInteger == rep a

    val(a: SingleInteger): % == per a

+++ `FiniteSet' is a domain defined by a set of strings. This should
+++ be an extension of `Enumeration' when the compiler gets fixed!
+++
+++ Author: ADK
+++ Date Created: 23-APR-1996 14:48:11.00
+++ Modifications

FiniteSet(ts: Tuple String): CountablyFinite SI with

    coerce: String -> %

    coerce: % -> String

  == add

    Rep == SI
    import from SI

    card: SI == length ts

    all(): Generator % == generate for i in 1 .. card repeat yield per i

    ord(a: %): SI == rep a

    val(i: SI): % ==
      i < 1 or i > card => error "val$FiniteSet: out of range"
      per i

    sample: % == per 1			-- What if card = 0?

    (a: %) = (b: %): Boolean == rep.a = rep.b

    (p: TextWriter) << (a: %): TextWriter == p << a::String

    coerce(a: %): String == element(ts, rep a)

    coerce(s: String): % ==
      for a: % in all() repeat s = a::String => return a
      error "coerce$FiniteSet: string not in set"
