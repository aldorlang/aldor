--* From aldor-1.1.12@dcs.st-and.ac.uk  Mon May 31 15:49:31 1999
--* Received: from knockdhu.cs.st-andrews.ac.uk ([138.251.206.239])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id PAA12821
--* 	for <ax-bugs@nag.co.uk>; Mon, 31 May 1999 15:49:30 +0100 (BST)
--* Received: from localhost (aldor-1.1.12@localhost)
--* 	by knockdhu.cs.st-andrews.ac.uk (8.8.7/8.8.7) with ESMTP id PAA13848;
--* 	Mon, 31 May 1999 15:52:16 +0100
--* X-Authentication-Warning: knockdhu.cs.st-andrews.ac.uk: aldor-1.1.12 owned process doing -bs
--* Date: Mon, 31 May 1999 15:52:16 +0100 (GMT)
--* From: "Aldor v1.1.12 clean development" <aldor-1.1.12@dcs.st-and.ac.uk>
--* X-Sender: aldor-1.1.12@knockdhu.cs.st-andrews.ac.uk
--* To: ax-bugs@nag.co.uk
--* cc: mnd@dcs.st-and.ac.uk
--* Subject: [9][scobind?] Strange behaviour when defining domains
--* Message-ID: <Pine.LNX.4.04.9905311550565.13799-100000@knockdhu.cs.st-andrews.ac.uk>
--* MIME-Version: 1.0
--* Content-Type: TEXT/PLAIN; CHARSET=US-ASCII
--* Content-ID: <Pine.LNX.4.04.9905311550567.13799@knockdhu.cs.st-andrews.ac.uk>

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


-- Command line: See header comment
-- Version: 1.1.12 for LINUX(glibc) (debug version)
-- Original bug file name: catBug.as


----------------------------------------------------------------------
-- Compare the output when compiled with and without inlining:
--
-- % axiomxl -Q2             -Grun catBug.as
-- % axiomxl -Q2 -Qno-inline -Grun catBug.as
--
-- The problem seems to be that if we define a domain which implements
-- a pre-defined category then the compiler believes that its exports
-- are those defined in the category not the domain. Simple changing
-- the category to `Cat with' allows the compiler to see each export
-- originating from the correct domain.
--
-- This also kills the inliner: if we get rid of the default then we
-- get the correct behaviour under any optimisation. However, all the
-- exports of A and B are inlined while those of X and Y are.
----------------------------------------------------------------------

#include "axllib"

SI ==> SingleInteger;

----------------------------------------------------------------------

define Cat:Category == with
{
   foo : () -> SI;

   default { foo():SI == 42; }
}

----------------------------------------------------------------------

A:Cat == add;


B:Cat == add
{
   import from SI;
   foo():SI == 84;
}

----------------------------------------------------------------------

X:Cat with == add;

Y:Cat with == add 
{
   import from SI;
   foo():SI == 84;
}

----------------------------------------------------------------------

main():() ==
{
   import from SI;

   print << "foo()$A = " << foo()$A;
   print << " (ought to be " << foo()$X << ")" << newline;

   print << "foo()$B = " << foo()$B;
   print << " (ought to be " << foo()$Y << ")" << newline;
}

main();

----------------------------------------------------------------------


