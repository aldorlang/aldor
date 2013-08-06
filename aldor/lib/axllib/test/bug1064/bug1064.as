--* Received: from inf.ethz.ch (neptune.ethz.ch) by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA21428; Thu, 18 Apr 96 19:55:54 BST
--* Received: from vinci.inf.ethz.ch (bronstei@vinci.inf.ethz.ch [129.132.12.46]) by inf.ethz.ch (8.6.10/8.6.10) with ESMTP id UAA02164 for <ax-bugs@nag.co.uk>; Thu, 18 Apr 1996 20:50:45 +0200
--* From: Manuel Bronstein <bronstei@inf.ethz.ch>
--* Received: (bronstei@localhost) by vinci.inf.ethz.ch (8.6.8/8.6.6) id UAA09350 for ax-bugs@nag.co.uk; Thu, 18 Apr 1996 20:50:44 +0200
--* Date: Thu, 18 Apr 1996 20:50:44 +0200
--* Message-Id: <199604181850.UAA09350@vinci.inf.ethz.ch>
--* To: ax-bugs
--* Subject: [2] mod_X with negative args --> bad results

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: none
-- Version: 1.1.6
-- Original bug file name: modneg.as

------------------------ modneg.as ---------------------
--
-- It look likes the various mod_X operations give wrong result when
-- one of the first 2 inputs is negative. Are negative inputs supposed
-- to be supported?
-- % axiomxl -fx modneg.as
-- % modneg
-- 2021724253 + 760744993 (mod 2147481359) = 634987887
-- 2021724253 - (-760744993) (mod 2147481359) = 634983309
--                                              ^^^^^^^^^ WRONG!

#include "axllib"

macro Z == SingleInteger;

import from Z;
a := 2021724253;
b := -760744993;
p := 2147481359;
good  := mod_+(a, -b, p);
bad   := mod_-(a, b, p);
print << a << " + " << -b << " (mod " << p << ") = " << good << newline;
print << a << " - (" << b << ") (mod " << p << ") = " << bad << newline;

