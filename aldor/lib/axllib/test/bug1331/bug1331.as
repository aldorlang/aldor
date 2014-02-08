--* From hemmecke@risc.uni-linz.ac.at  Fri Sep 28 13:36:07 2001
--* Received: from welly-4.star.net.uk (welly-4.star.net.uk [195.216.16.162])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id NAA17335
--* 	for <ax-bugs@nag.co.uk>; Fri, 28 Sep 2001 13:36:03 +0100 (BST)
--* From: hemmecke@risc.uni-linz.ac.at
--* Received: (qmail 18812 invoked from network); 28 Sep 2001 12:35:33 -0000
--* Received: from 1.star-private-mail-12.star.net.uk (HELO smtp-in-1.star.net.uk) (10.200.12.1)
--*   by 204.star-private-mail-4.star.net.uk with SMTP; 28 Sep 2001 12:35:33 -0000
--* Received: (qmail 8900 invoked from network); 28 Sep 2001 12:35:33 -0000
--* Received: from mail17.messagelabs.com (62.231.131.67)
--*   by smtp-in-1.star.net.uk with SMTP; 28 Sep 2001 12:35:33 -0000
--* X-VirusChecked: Checked
--* Received: (qmail 22635 invoked from network); 28 Sep 2001 12:32:30 -0000
--* Received: from kernel.risc.uni-linz.ac.at (193.170.37.225)
--*   by server-13.tower-17.messagelabs.com with SMTP; 28 Sep 2001 12:32:30 -0000
--* Received: from enceladus.risc.uni-linz.ac.at (enceladus.risc.uni-linz.ac.at [193.170.37.81])
--* 	by kernel.risc.uni-linz.ac.at (8.11.6/8.11.6) with ESMTP id f8SCZMb11873;
--* 	Fri, 28 Sep 2001 14:35:22 +0200
--* Message-ID: <XFMail.20010928143522.hemmecke@risc.uni-linz.ac.at>
--* X-Mailer: XFMail 1.4.4 on Linux
--* X-Priority: 3 (Normal)
--* Content-Type: text/plain; charset=us-ascii
--* Content-Transfer-Encoding: 8bit
--* MIME-Version: 1.0
--* Date: Fri, 28 Sep 2001 14:35:22 +0200 (CEST)
--* Sender: hemmecke@risc.uni-linz.ac.at
--* To: ax-bugs@nag.co.uk
--* Subject: [5]non-constant constants and domain construction

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -V -fx xxx.as
-- Version: Aldor version 1.1.13p1(5) for LINUX(glibc2.1)
-- Original bug file name: xxx.as

-- Author: Ralf Hemmecke, Johannes Kepler Universit"at Linz
-- Date: 27-SEP-2001
-- Aldor version 1.0.-1(3) for LINUX(glibc2.1) 
-- Subject: [5]non-constant constants and domain construction

-- Compilation with
--        aldor -laxllib -grun xxx.as
-- will be fine. Option -DCB points to the problem I had in mind.
--  (name, fun) == method;
-- appears in a for loop and thus I cannot look at `name' as a
-- constant but rather would call it a variable since it can change
-- its value.

-- At least with the compiler I have, the -DCA -grun option shows
-- unprintable characters for the foo() call like this...

--: Trying identity...
--: This is Foo <σÿΏ4Έ__πέ__x$
--rhx: (... rest removed ...)

#include "axllib"

I ==> SingleInteger;

#if CA
Foo(s: String): with {
        foo: () -> ();
} == add {
        foo(): () == error << "This is Foo " << s << newline;
}
#endif

FunChoice: with {
        choose: String -> (I -> I);
} == add {
        NamedFun ==> Cross(a:String, b: I->I);
        import from String, I;
        NotFound:with == add;

        local identity(x:I):I == x;
        local double(x:I):I == x + x;
        local product(x:I):I == x * x;

        methods:List NamedFun == [
                ("identity", identity),
                ("double", double),
                ("product", product)
        ];

        choose(m:String):(I -> I) == {
#if CB
                local name: String == "NEW";
#endif
                import from List(NamedFun);
                for method in methods repeat {
                        (name: String, fun: I->I) == method;
                        print << "Trying " << name << "..." << newline;
                        if (m = name) then {
#if CA
                                foo()$Foo(name);
#endif
                                return fun;
                        }
                }
                throw NotFound;
        }
}

main():() == {
        import from FunChoice, I;
	import from GeneralAssert;
        funs:List String := ["identity", "double", "product"];
        nofuns:List String := ["foo", "bar"];

        for fname in funs repeat {
                fun := choose(fname);
                print << fname << "(" << 12 << ") = " << fun(12) << newline;
        }
        for fname in nofuns repeat {
                assertFail((): () +-> choose(fname));
        }
}

main();
