--* From youssef@d0lxbld4.fnal.gov  Sun Jul 23 04:46:48 2000
--* Received: from d0lxbld4.fnal.gov (d0lxbld4.fnal.gov [131.225.225.19])
--* 	by nagmx1.nag.co.uk (8.9.3/8.9.3) with ESMTP id EAA14105
--* 	for <ax-bugs@nag.co.uk>; Sun, 23 Jul 2000 04:46:46 +0100 (BST)
--* Received: (from youssef@localhost)
--* 	by d0lxbld4.fnal.gov (8.9.3/8.9.3) id WAA14992
--* 	for ax-bugs@nag.co.uk; Sat, 22 Jul 2000 22:09:45 -0500
--* Date: Sat, 22 Jul 2000 22:09:45 -0500
--* From: Saul Youssef <youssef@d0lxbld4.fnal.gov>
--* Message-Id: <200007230309.WAA14992@d0lxbld4.fnal.gov>
--* To: ax-bugs@nag.co.uk
--* Subject: [4] "hello world" core dumps in RH 6.2

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -grun -laxllib hello.as
-- Version: 1.1.12p5
-- Original bug file name: hello.as

--+ 
--+ Hi Martin,
--+ 
--+    I've set myself up with Red Hat 6.2 and I'm testing 12p5.  The "hello"
--+ program below works fine with Red Hat 5.2.1 but core dumps for Red Hat 6.2.
--+ 
--+    I've got myself setup with Redhat 6.2 and I'm testing 12p5.  This refers 
--+ to the core dumping problem with
--+ 
--+ #include "axllib"
--+ #pile
--+ print << " hello " << newline
--+ 
--+ It works fine with
--+ 
--+   % axiomxl -ginterp hello.as
--+   
--+ With 
--+ 
--+   % axiomxl -fx -Faxlmain -Fc -Zdb -laxllib hello.as
--+   % gdb hello
--+   (gdb) run
--+   
--+ I get the following:
--+ 
--+ Starting program: /home/youssef/sw/cats/hello
--+ 
--+ Program received signal SIGSEGV, Segmentation fault.
--+ 0x40089af2 in _IO_fputs (str=0x80984a0 " hello ", fp=0x80a5b50) at iofputs.c:39
--+ 39       iofputs.c: No such file or directory
--+ (gdb)
--+ 
--+ Saul
--+ 
--+ 
--
--  The following program core dumps with
--
--   axiomxl -grun -laxllib hello.as 
--
--  under Red Hat Linux 6.2.  It works fine for Red Hat 5.2
--
--   Saul Youssef
--
#include "axllib"

print << " hello " << newline

