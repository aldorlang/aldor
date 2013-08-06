--* Received: from mailer.scri.fsu.edu by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA05811; Fri, 12 Apr 96 05:19:51 BST
--* Received: from ibm4.scri.fsu.edu (ibm4.scri.fsu.edu [144.174.131.4]) by mailer.scri.fsu.edu (8.6.12/8.6.12) with SMTP id AAA04944; Fri, 12 Apr 1996 00:16:19 -0400
--* From: Tony Kennedy <adk@scri.fsu.edu>
--* Received: by ibm4.scri.fsu.edu (5.67b) id AA11737; Fri, 12 Apr 1996 00:14:41 -0400
--* Date: Fri, 12 Apr 1996 00:14:41 -0400
--* Message-Id: <199604120414.AA11737@ibm4.scri.fsu.edu>
--* To: adk@scri.fsu.edu, ax-bugs, edwards@scri.fsu.edu
--* Subject: [3] (1) Compiler bug, (2) Error flagged for correct code

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: (none)
-- Version: 1.1.5
-- Original bug file name: bug-conditional-export.as

--+ ibm4::adk> axiomxl -V -Ginterp -DERROR1 -Mno-mactext bug-conditional-export.as
--+ AXIOM-XL version 1.1.5 for AIX RS/6000 
--+ "bug-conditional-export.as", line 25 char 11Compiler bug...Bug: Bad case 9 (line 1627 in file ../src/absyn.c).
--+ ibm4::adk> axiomxl -V -Ginterp -DERROR2 -Mno-mactext bug-conditional-export.as
--+ AXIOM-XL version 1.1.5 for AIX RS/6000 
--+ "bug-conditional-export.as", line 72:         commutator(x: %, y: %): % == 1
--+                                       ........^
--+ [L72 C9] #2 (Note 1)  (cf. L74 C9)
--+ 
--+ "bug-conditional-export.as", line 74: 
--+         commutator(x: %, y: %): % == x * inv.y
--+ ........^
--+ [L74 C9] #1 (Error) Constant `commutator' cannot be redefined.
--+ (see Note 1)
--+ 
--+                ld in sc sy li pa ma ab ck sb ti gf of pb pl pc po mi
--+  Time    0.3 s  0 13  9 25  6 13  0  6  0 22  0  0  0  0  0  0  0  6 %
--+ 
--+  Source  292 lines,  54750 lines per minute
--+  Store   348 K pool
--+ ibm4::adk> axiomxl -V -Ginterp -Mno-mactext bug-conditional-export.as
--+ AXIOM-XL version 1.1.5 for AIX RS/6000 
--+                ld in sc sy li pa ma ab ck sb ti gf of pb pl pc po mi
--+  Time    4.7 s  0  1 .4  1 .4  1 .2 .2  0  1 78  6  7  2  0  0  0 .4 %
--+ 
--+  Source  292 lines,  3719 lines per minute
--+  Lib   25500 bytes,  3318syme 16766foam 366fsyme 922name 158kind 513file 506lazy 2745type 2inl 14twins 14ext 2doc 27id
--+  Store  3408 K pool
--+ commutator([ 2 1 3 ],[ 1 3 2 ]) = [ 2 3 1 ]
#include "axllib.as"
#pile

SI ==> SingleInteger

Abelian: Category == with

Funny: Category == with

-- Symmetric group on n objects

S(n: SI): Join(Group, Funny)
  with

      flip: (SI, SI) -> %

    == add

      Rep == Array SI
      import from SI, Rep

      flip(i: SI, j: SI): % ==
        a: Rep == [k for k in 1 .. n]
#if ERROR1
        (a.i, a.j) := (a.j, a.i)	-- Exercises compiler bug
#else
        swap == a.i
        a.i := a.j
        a.j := swap
#endif
        per a

      1: % == per [i for i in 1@SI .. n]

      (a: %) ^ (n: Integer): % == power(1, a, n)$BinaryPowering(%, *, Integer)

      sample: % == 1

      (a: %) = (b: %): Boolean == rep.a = rep.b

      (a: %) * (b: %): % == per [(rep a)((rep.b).i) for i in 1 .. n]

      inv(a: %): % ==
	b: Rep == new(n, 0)
	for i in 1 .. n repeat b((rep a).i) := i
	per b

      (p: TextWriter) << (a: %): TextWriter ==
	p << "[ "
	for i in rep a repeat p << i << " "
	p << "]"

CGroup: Category == Group with

  commutator: (%, %) -> %

  default
    commutator(g: %, h: %): % == g * h * inv.g * inv.h

#if ERROR2
D(X: Group): CGroup with

    coerce: X -> %

  == X add

    Rep ==> X

    coerce(x: X): % == per x

    if X has Abelian then
	commutator(x: %, y: %): % == 1
      else if X has Funny then
	commutator(x: %, y: %): % == x * inv.y
#endif

E(X: Group): CGroup with

    coerce: X -> %

  == X add

    Rep ==> X

    abelian? ==> X has Abelian
    funny? ==> X has Funny

    coerce(x: X): % == per x

    if abelian? or funny? then
      commutator(x: %, y: %): % ==
	abelian? => 1
	funny? => x * inv.y
	never

test(): () ==

  import from SI
  S3 == S(3)
  EE == E(S3)
  s12: S3 == flip(1, 2)
  s23: S3 == flip(2, 3)
  e12: EE == coerce s12
  e23: EE == coerce s23
  ee: EE == commutator(e12, e23)

  print."commutator(~a,~a) = ~a~n".(<< e12, << e23, << ee)

test()
