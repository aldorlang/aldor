--* From Manuel.Bronstein@sophia.inria.fr  Thu Jun 14 17:37:15 2001
--* Received: from server-7.tower-4.starlabs.net (mail.london-1.starlabs.net [212.125.75.12])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id RAA29962
--* 	for <ax-bugs@nag.co.uk>; Thu, 14 Jun 2001 17:37:14 +0100 (BST)
--* X-VirusChecked: Checked
--* Received: (qmail 12723 invoked from network); 14 Jun 2001 16:36:32 -0000
--* Received: from droopix.inria.fr (138.96.111.4)
--*   by server-7.tower-4.starlabs.net with SMTP; 14 Jun 2001 16:36:32 -0000
--* Received: by droopix.inria.fr (8.11.1/8.10.0) id f5EGagD22414 for ax-bugs@nag.co.uk; Thu, 14 Jun 2001 18:36:42 +0200
--* Date: Thu, 14 Jun 2001 18:36:42 +0200
--* From: Manuel Bronstein <Manuel.Bronstein@sophia.inria.fr>
--* Message-Id: <200106141636.f5EGagD22414@droopix.inria.fr>
--* To: ax-bugs@nag.co.uk
--* Subject: [3] condition kills inlining

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -q5 -fc -qinline-all optcond.as
-- Version: 1.1.13
-- Original bug file name: optcond.as

---------------------------- optcond.as -------------------------------
-- 
-- Conditional definitions seem never to be inlined in clients
-- when alternatives are present:
-- static FiWord CF6_f(FiEnv e1, FiWord P0_n)
-- {
--         PFmt10 l1;
--         l1 = (PFmt10) fiEnvLevel(e1);
--         return fiCCall1(FiWord, l1->X2_foo, P0_n);
-- }
--
-- However, with the 'else' clause commented out, we get:
-- static FiWord
-- CF5_f(FiEnv e1, FiWord P0_n)
-- {
--         return (FiWord) -(FiSInt) P0_n;
-- }

#include "axllib"

Foo(T:BasicType): with { foo: T -> T } == add {
	if T has ArithmeticSystem then { foo(t:T):T == -t }
				else { foo(t:T):T == t }
}

f(n:SingleInteger):SingleInteger == {
	import from Foo SingleInteger;
	foo n;
}


