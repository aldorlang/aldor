--* From mnd@knockdhu.cs.st-andrews.ac.uk  Thu Jul 22 12:43:42 1999
--* Received: from knockdhu.cs.st-andrews.ac.uk ([138.251.206.239])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id MAA25200
--* 	for <ax-bugs@nag.co.uk>; Thu, 22 Jul 1999 12:43:25 +0100 (BST)
--* Received: (from mnd@localhost)
--* 	by knockdhu.cs.st-andrews.ac.uk (8.8.7/8.8.7) id MAA02030
--* 	for ax-bugs@nag.co.uk; Thu, 22 Jul 1999 12:43:50 +0100
--* Date: Thu, 22 Jul 1999 12:43:50 +0100
--* From: mnd <mnd@knockdhu.cs.st-andrews.ac.uk>
--* Message-Id: <199907221143.MAA02030@knockdhu.cs.st-andrews.ac.uk>
--* To: ax-bugs@nag.co.uk
--* Subject: [9] unicl can't produce an executable called `for'!

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -Fx for.as
-- Version: 1.1.12p3 for LINUX(glibc) (debug version)
-- Original bug file name: for.as

--+ 
--+ For some strange reason unicl can't produce an executable with the name
--+ "for". So, given a simple program "hello.c", under Linux we get:
--+ 
--+    % unicl -o for hello.c
--+    ld: target /usr/lib/crt1.o not found
--+ 
--+ This means that one cannot compile Aldor programs whose main entry point
--+ appears in a file called "for.as":
--+ 
--+    % axiomxl -Fx for.as
--+    ld: target /usr/lib/crt1.o not found
--+    #1 (Fatal Error) Linker failed.  Command was: unicl for.o axlmain.o -L. 
--+    -L/home/mnd/nag/compiler/aldor/1.1.12/base/linux/share/lib
--+    -L/home/mnd/nag/compiler/aldor/1.1.12/base/linux/lib
--+    -o for -laxllib -lfoam
--+    #1 (Warning) Removing file `for.o'.
--+    #2 (Warning) Removing file `axlmain.o'.
--+ 

#include "axllib"

print << "Hello world!" << newline;

