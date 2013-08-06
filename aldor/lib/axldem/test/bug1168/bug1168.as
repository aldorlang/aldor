--* From axiom@nag.co.uk  Fri Sep 17 09:58:59 1999
--* Received: from red.nag.co.uk (red.nag.co.uk [192.156.217.2])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with SMTP id JAA13940
--* 	for <ax-bugs@nag.co.uk>; Fri, 17 Sep 1999 09:58:59 +0100 (BST)
--* Received: from zorba.nag.co.uk by red.nag.co.uk via SMTP (920330.SGI/920502.SGI)
--* 	for ax-bugs@nag.co.uk id AA05342; Fri, 17 Sep 99 09:54:01 +0100
--* Received: by zorba.nag.co.uk (8.8.7/920502.SGI)
--* 	for ax-bugs@nag.co.uk id JAA29162; Fri, 17 Sep 1999 09:51:38 +0100
--* Date: Fri, 17 Sep 1999 09:51:38 +0100
--* From: axiom@nag.co.uk (Axiom Implementation Account)
--* Message-Id: <199909170851.JAA29162@zorba.nag.co.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [3][optimiz] inliner messes up environment

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -q3 -fx gbtest1.as
-- Version: 1.1.12
-- Original bug file name: test/gbtest1.as

-- Copyright The Numerical Algorithms Group Limited 1996.
--> testint
--> testrun -O -l axldem
--> testcomp -OQinline-limit=1000

#pile
#include "axllib.as"
#library DemoLib       "axldem"
import from DemoLib

R ==> Integer
import from R
hdp ==> HomogeneousDirectProduct(retract 2)
import from hdp
poly ==> Polynomial(R, hdp)
import from poly
x:= var unitVector retract 1
y:= var unitVector retract 2
p1:=x+y
p2:=x-y
q1:=p1*p2
q2:=p1*p1
l1:List poly := [q1,q2]
print<<l1<<newline
import from GroebnerPackage(R, hdp, poly)
import from String
base := groebner(l1,"info")
print<<base<<newline
f1:=x^4-3*x^2*y^2 + y^4
f2:=x*y^3-x^3*y
l2:List poly := [f1,f2]
print<<l2<<newline
base := groebner(l2)
print<<base<<newline

