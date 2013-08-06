--* From bronstei@inf.ethz.ch  Thu Dec 19 13:40:17 1996
--* Received: from nagmx1.nag.co.uk by red.nag.co.uk via SMTP (920330.SGI/920502.SGI)
--* 	for /home/red5/axiom/support/recvbug id AA27797; Thu, 19 Dec 96 13:40:17 GMT
--* Received: from inf.ethz.ch (root@neptune.ethz.ch [129.132.10.10])
--*           by nagmx1.nag.co.uk (8.8.4/8.8.4) with SMTP
--* 	  id NAA04928 for <ax-bugs@nag.co.uk>; Thu, 19 Dec 1996 13:41:43 GMT
--* Received: from mendel.inf.ethz.ch (bronstei@mendel.inf.ethz.ch [129.132.12.20]) by inf.ethz.ch (8.6.10/8.6.10) with ESMTP id OAA00393 for <ax-bugs@nag.co.uk>; Thu, 19 Dec 1996 14:41:01 +0100
--* From: Manuel Bronstein <bronstei@inf.ethz.ch>
--* Received: (from bronstei@localhost) by mendel.inf.ethz.ch (8.7.1/8.7.1) id OAA16447 for ax-bugs@nag.co.uk; Thu, 19 Dec 1996 14:41:00 +0100 (MET)
--* Date: Thu, 19 Dec 1996 14:41:00 +0100 (MET)
--* Message-Id: <199612191341.OAA16447@mendel.inf.ethz.ch>
--* To: ax-bugs@nag.co.uk
--* Subject: [3] UNICL variable poorly parsed

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: none
-- Version: 1.1.8a
-- Original bug file name: hello.as

-------------------------- hello.as ---------------------
--
-- This shows a bug in the parsing of the UNICL C-compiler option.
-- The gcc option I want to pass is  -mv8, but unicl passes it as -m v8
-- which is not at all the same thing:
--
-- % setenv UNICL -mv8
-- % echo $UNICL
-- -mv8
-- % axiomxl -fx -v hello.as
-- AXIOM-XL version 1.1.8 for SPARC [Solaris: GCC] 
-- Exec: unicl -I/homes/asharp/axiomxl/solaris/include -c hello.c
-- Exec: gcc -I/homes/asharp/axiomxl/solaris/include -I/usr/include -c -m v8 hello.c
-- cc1: v8: No such file or directory
-- [... many lines suppressed ...]
-- gcc25: hello.o: No such file or directory
-- gcc25: axlmain.o: No such file or directory

#include "axllib"

print << "hello world" << newline;
