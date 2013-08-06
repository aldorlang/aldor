--* From Manuel.Bronstein@sophia.inria.fr  Mon Aug  5 15:25:06 2002
--* Received: from welly-3.star.net.uk (welly-3.star.net.uk [195.216.16.161])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id PAA02041
--* 	for <ax-bugs@nag.co.uk>; Mon, 5 Aug 2002 15:25:05 +0100 (BST)
--* Received: (qmail 29769 invoked from network); 5 Aug 2002 14:24:23 -0000
--* Received: from 6.star-private-mail-12.star.net.uk (HELO smtp-in-6.star.net.uk) (10.200.12.6)
--*   by delivery-3.star-private-mail-4.star.net.uk with SMTP; 5 Aug 2002 14:24:23 -0000
--* Received: (qmail 22936 invoked from network); 5 Aug 2002 14:24:23 -0000
--* Received: from mail17.messagelabs.com (62.231.131.67)
--*   by smtp-in-6.star.net.uk with SMTP; 5 Aug 2002 14:24:23 -0000
--* X-VirusChecked: Checked
--* Received: (qmail 22767 invoked from network); 5 Aug 2002 14:24:19 -0000
--* Received: from panoramix.inria.fr (138.96.111.9)
--*   by server-13.tower-17.messagelabs.com with SMTP; 5 Aug 2002 14:24:19 -0000
--* Received: (from bmanuel@localhost)
--* 	by panoramix.inria.fr (8.12.5/8.12.5) id g75EOJjM006726
--* 	for ax-bugs@nag.co.uk; Mon, 5 Aug 2002 16:24:19 +0200
--* Date: Mon, 5 Aug 2002 16:24:19 +0200
--* From: Manuel Bronstein <Manuel.Bronstein@sophia.inria.fr>
--* Message-Id: <200208051424.g75EOJjM006726@panoramix.inria.fr>
--* To: ax-bugs@nag.co.uk
--* Subject: [3] yet another optimizer bug

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: aldor -ginterp optbug.as
-- Version: 1.0.0
-- Original bug file name: optbug.as

-------------------------------- optbug.as -----------------------------
--
-- Yet another optimized bug, somewhere in sit_dmp1 or sit_dmp0
--
-- % aldor -ginterp optbug.as
-- Here I was ... 1
-- Program fault (segmentation violation).#2 (Error) Program fault (seg...)
--
-- % aldor -ginterp -dDEBUG optbug.as
-- Here I was ... 1
-- Here I was ... 2
-- Here I was ... 3
-- x^5 + 5*x^4*y + ...
--

#include "algebra"
#include "aldorio"

import from String, Symbol;

macro V == OrderedVariableTuple ( -"x", -"y", -"z");

import from MachineInteger, Integer, V;

MI == MachineInteger;
I == Integer;

macro E == MachineIntegerDegreeLexicographicalExponent(V);
macro P == DistributedMultivariatePolynomial1(Integer, V, E);

x: P := variable(1)$V :: P;
y: P := variable(2)$V :: P;
z: P := variable(3)$V :: P;

stdout << "Here I was ... 1" << newline;

p: P := (1+x+y+z)^(5::MI);

stdout << "Here I was ... 2" << newline;

p1 := p+1;

stdout << "Here I was ... 3" << newline;

stdout << p1 << newline;
