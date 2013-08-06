--* From Manuel.Bronstein@sophia.inria.fr  Fri Oct 19 15:22:23 2001
--* Received: from welly-4.star.net.uk (welly-4.star.net.uk [195.216.16.162])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id PAA06835
--* 	for <ax-bugs@nag.co.uk>; Fri, 19 Oct 2001 15:22:22 +0100 (BST)
--* Received: (qmail 28270 invoked from network); 19 Oct 2001 14:21:53 -0000
--* Received: from 6.star-private-mail-12.star.net.uk (HELO smtp-in-6.star.net.uk) (10.200.12.6)
--*   by 204.star-private-mail-4.star.net.uk with SMTP; 19 Oct 2001 14:21:53 -0000
--* Received: (qmail 19697 invoked from network); 19 Oct 2001 14:21:52 -0000
--* Received: from mail17.messagelabs.com (62.231.131.67)
--*   by smtp-in-6.star.net.uk with SMTP; 19 Oct 2001 14:21:52 -0000
--* X-VirusChecked: Checked
--* Received: (qmail 22137 invoked from network); 19 Oct 2001 13:45:24 -0000
--* Received: from panoramix.inria.fr (138.96.111.9)
--*   by server-5.tower-17.messagelabs.com with SMTP; 19 Oct 2001 13:45:24 -0000
--* Received: by panoramix.inria.fr (8.11.6/8.10.0) id f9JDoUe18914 for ax-bugs@nag.co.uk; Fri, 19 Oct 2001 15:50:30 +0200
--* Date: Fri, 19 Oct 2001 15:50:30 +0200
--* From: Manuel Bronstein <Manuel.Bronstein@sophia.inria.fr>
--* Message-Id: <200110191350.f9JDoUe18914@panoramix.inria.fr>
--* To: ax-bugs@nag.co.uk
--* Subject: [7] bad inlining by optimizer

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: aldor -q2 -fx wrongopt.as
-- Version: 1.0.-2
-- Original bug file name: wrongopt.as

-------------------------- wrongopt.as -------------------------------
--
-- % aldor -q1 -ginterp wrongopt.as
-- g = 1  ----> [GOOD]
--
-- % aldor -q2 -ginterp wrongopt.as
-- g = Program fault (segmentation violation).
--
-- % aldor -q1 -fx -lalgebra -laldor wrongopt.as
-- % wrongopt
-- g = 1  ----> [GOOD]
--
-- % aldor -q2 -fx -lalgebra -laldor wrongopt.as
-- % wrongopt
-- g = 3\,x^{6}+5\,x^{4}-4\,x^{2}-9\,x+21  ----> [BAD]
--

#include "algebra"
#include "aldorio"

macro {
        Z == Integer;
        Zx == DenseUnivariatePolynomial(Z, -"x");
}

local gcd():() == {
	import from Z, Symbol, Zx;
	import from Partial Zx, ModularUnivariateGcd(Z, Zx);

	x := monom;
	p := x^8 + x^6 - 3*x^4 - 3*x^3 + 8*x^2 +2*x - 5@Z ::Zx;
	q := 3*x^6 + 5*x^4 -4*x^2 -9*x + 21@Z ::Zx;
	(u, a, b) := modularGcd(p, q);
	g := retract u;
	stdout << "g = " << g << "  ----> ";
	if one? g then stdout << "[GOOD]" << endnl;
		else stdout << "[BAD]" << endnl;
}

gcd();

